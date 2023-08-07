#include "Characters/BaseCharacter.h"
#include "Characters/SlashCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Slash/DebugMacros.h"
#include "GameModes/SlashGameMode.h"

//-----------------------------------------------------------------------------
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

//-----------------------------------------------------------------------------
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

//-----------------------------------------------------------------------------
void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (SHOW_DEBUG)
	{
		DRAW_SPHERE(ImpactPoint, FColor::Orange, 5.f, 5.f);
	}

	if (IsAlive())
	{
		if (Hitter)
		{
			DirectionalHitReact(Hitter->GetActorLocation());
		}
	}
	else
	{
		Die();
	}

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

//-----------------------------------------------------------------------------
void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(cTagDead))
	{
		CombatTarget = nullptr;
	}
}

//-----------------------------------------------------------------------------
void ABaseCharacter::AttackHard()
{
	if (CombatTarget && CombatTarget->ActorHasTag(cTagDead))
	{
		CombatTarget = nullptr;
	}
}

//-----------------------------------------------------------------------------
void ABaseCharacter::SetAttackingEnemy(AActor* Enemy)
{
	CombatTarget = Enemy;
}

//-----------------------------------------------------------------------------
void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

//-----------------------------------------------------------------------------
FVector ABaseCharacter::GetTranslationWrapTarget()
{
	if (CombatTarget == nullptr)
	{
		return FVector();
	}
	
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector AttackerLocation = GetActorLocation();
	
	FVector TargetToAttacker = (AttackerLocation - CombatTargetLocation).GetSafeNormal();
	TargetToAttacker *= WarpTargetDistance;
	if (ASlashGameMode::GetDebugState(GetWorld(), EDebugElement::EAS_Warping))
	{
		if (Cast<ASlashCharacter>(this))
		{
			DRAW_SPHERE(CombatTargetLocation + TargetToAttacker, FColor::Yellow, 25.f, 5.f);
		}
		else
		{
			DRAW_SPHERE(CombatTargetLocation + TargetToAttacker, FColor::Green, 25.f, 5.f);
		}
	}
	return CombatTargetLocation + TargetToAttacker;	
}

//-----------------------------------------------------------------------------
FVector ABaseCharacter::GetRotationWrapTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

//-----------------------------------------------------------------------------
void ABaseCharacter::PlayEquipMontage(const FName& SectionName)
{
}

void ABaseCharacter::DodgeEnd()
{
}

void ABaseCharacter::AttackEnd()
{
}

bool ABaseCharacter::CanAttack() const
{
	return false;
}

bool ABaseCharacter::CanDisarm() const
{
	return false;
}

bool ABaseCharacter::CanArm() const
{
	return false;
}

bool ABaseCharacter::IsAlive() const
{
	return Attributes && Attributes->IsAlive();
}

//-----------------------------------------------------------------------------
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//-----------------------------------------------------------------------------
void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnorActors.Empty();
	}
}

//-----------------------------------------------------------------------------
void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			ImpactPoint);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

//-----------------------------------------------------------------------------
void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

//-----------------------------------------------------------------------------
void ABaseCharacter::Die_Implementation()
{
	PlayDeathMontage();
	DisableCapsule();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	Tags.Add(cTagDead);
}

//-----------------------------------------------------------------------------
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLower(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLower - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1 |ToHit| =1 , so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the arc cosine of cos (theta) to get theta
	double Theta = FMath::Acos(CosTheta); // get in radian
	//Convert from radian to degree
	Theta = FMath::RadiansToDegrees(Theta);

	// if CrossProduct points down , theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("FromBack");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	PlayHitReactMontage(Section);

	// DEBUG
	if (SHOW_DEBUG)
	{
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
			GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, Section.ToString());
		}
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 100.f, 5.f, FColor::Red, 10.f, 3.f);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 100.f, 5.f, FColor::Green, 10.f, 3.f);
	}
}

//--------------------------------------------------------------------
int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

//--------------------------------------------------------------------
int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Selection < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

//--------------------------------------------------------------------
void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
