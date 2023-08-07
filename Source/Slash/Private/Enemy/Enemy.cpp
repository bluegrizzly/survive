#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Slash/DebugMacros.h"
#include "Items/Soul.h"
#include "GameModes/SlashGameMode.h"

//-----------------------------------------------------------------------------
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

//-----------------------------------------------------------------------------
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
	InitializeEnemy();
}

//-----------------------------------------------------------------------------
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}

	if (ASlashGameMode::GetDebugState(GetWorld(), EDebugElement::EAS_Combat))
	{
		DRAW_SPHERE_SingleFrame(GetActorLocation(), FColor::Orange, CombatRadius);
		DRAW_SPHERE_SingleFrame(GetActorLocation(), FColor::Red, AttackRadius);
	}
}

//-----------------------------------------------------------------------------
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}
	return DamageAmount;
}

//-----------------------------------------------------------------------------
void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

//-----------------------------------------------------------------------------
void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged())
		{
			StartPatrolling();
		}
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged())
		{
			ChaseTarget();
		}
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

//-----------------------------------------------------------------------------
void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float TimerLength = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, TimerLength);
	}
}

//-----------------------------------------------------------------------------
void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) 
	{
		ShowHealtBar();
	}
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	StopAttackMontage();
	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
}

//-----------------------------------------------------------------------------
void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		TObjectPtr<AWeapon> Weapon = World->SpawnActor<AWeapon>(WeaponClass);
		Weapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = Weapon;
	}
}

//-----------------------------------------------------------------------------
void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	HideHealthBar();
	Tags.Add(cTagEnemy);
	PatrolTarget = ChoosePatrolTarget();

	StartPatrolling();
	SpawnDefaultWeapon();
}

//-----------------------------------------------------------------------------
void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();

	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	SetPlayerAttaker(nullptr);
	HideHealthBar();
	SetLifeSpan(DeathLifeSpan);
	SpawnSoul();
}

//-----------------------------------------------------------------------------
void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());
			SpawnedSoul->SetOwner(this);
		}
	}
}

//-----------------------------------------------------------------------------
void AEnemy::Attack()
{
	Super::Attack();

	if (CombatTarget == nullptr) return;

	SetPlayerAttaker(CombatTarget);

	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

//-----------------------------------------------------------------------------
void AEnemy::SetPlayerAttaker(AActor* Attacker)
{
	// Tell the player who is attacking 
	ABaseCharacter* TargetCharacter = Attacker == nullptr ? Cast<ABaseCharacter> (CombatTarget) : Cast<ABaseCharacter>(Attacker);
	if (TargetCharacter)
	{
		TargetCharacter->SetAttackingEnemy(Attacker);
	}
}

//-----------------------------------------------------------------------------
bool AEnemy::CanAttack() const
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
	return bCanAttack;
}

//-----------------------------------------------------------------------------
void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

//-----------------------------------------------------------------------------
void AEnemy::PatrolTimerFinished()
{
	StartPatrolling();
}

//-----------------------------------------------------------------------------
void AEnemy::LoseInterest()
{
	// Outside combat radius, lose interset
	SetPlayerAttaker(nullptr);
	CombatTarget = nullptr;
	HideHealthBar();
}

//-----------------------------------------------------------------------------
void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

//-----------------------------------------------------------------------------
bool AEnemy::IsOutsideCombatRadius() const
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

//-----------------------------------------------------------------------------
void AEnemy::ChaseTarget()
{
	// Outside attack range, chase character
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

//-----------------------------------------------------------------------------
bool AEnemy::IsChasing() const
{
	return EnemyState == EEnemyState::EES_Chasing;
}

//-----------------------------------------------------------------------------
bool AEnemy::IsInsideAttackRadius() const
{
	return InTargetRange(CombatTarget, AttackRadius);
}

//-----------------------------------------------------------------------------
bool AEnemy::IsAttacking() const
{
	return EnemyState == EEnemyState::EES_Attacking;
}

//-----------------------------------------------------------------------------
bool AEnemy::IsDead() const
{
	return EnemyState == EEnemyState::EES_Dead;
}

//-----------------------------------------------------------------------------
bool AEnemy::IsEngaged() const
{
	return EnemyState == EEnemyState::EES_Engaged;
}

//-----------------------------------------------------------------------------
void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

//-----------------------------------------------------------------------------
void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

//-----------------------------------------------------------------------------
void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

//-----------------------------------------------------------------------------
bool AEnemy::IsOutsideAttackRadius() const
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

//-----------------------------------------------------------------------------
AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.Add(Target);
		}
	}

	if (ValidTargets.Num() > 0)
	{
		int32 PatrolIndex = FMath::RandRange(0, ValidTargets.Num() - 1);
		return ValidTargets[PatrolIndex];

	}
	return nullptr;
}

//-----------------------------------------------------------------------------
void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking&&
		SeenPawn->ActorHasTag(cTagEngagableTarget);

	if (bShouldChaseTarget)
	{
		SetAttackingEnemy(SeenPawn);
		ClearPatrolTimer();
		ChaseTarget();
	}
}

//-----------------------------------------------------------------------------
bool AEnemy::InTargetRange(AActor* Target, double Radius) const
{
	if (Target == nullptr) return false;
	const double  DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	if (SHOW_DEBUG_AI)
	{
		DRAW_SPHERE_SingleFrame(GetActorLocation(), FColor::Red, 25.f);
		DRAW_SPHERE_SingleFrame(Target->GetActorLocation(), FColor::Red, 25.f);
		DRAW_VECTOR_SingleFrame(GetActorLocation(), Target->GetActorLocation(), FColor::Red);
	}

	return DistanceToTarget <= Radius;
}

//-----------------------------------------------------------------------------
void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	FNavPathSharedPtr NavPath;
	EnemyController->MoveTo(MoveRequest, &NavPath);

	if (SHOW_DEBUG_AI)
	{
		TArray<FNavPathPoint>& PathPoint = NavPath->GetPathPoints();
		for (auto& Point : PathPoint)
		{
			const FVector& Location = Point.Location;
			DRAW_SPHERE(Location, FColor::White, 12.f, 10.f);
		}
	}
}

//-----------------------------------------------------------------------------
void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

//-----------------------------------------------------------------------------
void AEnemy::ShowHealtBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

