#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimInstance.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/HealthPickup.h"

//-----------------------------------------------------------------------------
ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

//-----------------------------------------------------------------------------
void ASlashCharacter::Tick(float DeltaTime)
{
	if (Attributes)
	{
		if (Attributes->RegenStamina(DeltaTime))
		{
			UpdateHUD();
		}
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(cTagEngagableTarget);

	InitializeSlashOverlay();
}

//-----------------------------------------------------------------------------
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ASlashCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
	PlayerInputComponent->BindAction(FName("AttackHard"), IE_Pressed, this, &ASlashCharacter::AttackHard);
	PlayerInputComponent->BindAction(FName("Dodge"), IE_Pressed, this, &ASlashCharacter::Dodge);
}

//-----------------------------------------------------------------------------
void ASlashCharacter::Jump()
{
	if (IsUnocupied())
	{
		Super::Jump();
	}
}

//-----------------------------------------------------------------------------
bool ASlashCharacter::IsUnocupied() const
{
	return ActionState == EActionState::EAS_Unoccupied;
}

//-----------------------------------------------------------------------------
bool ASlashCharacter::IsOccupied() const
{
	return ActionState != EActionState::EAS_Unoccupied;
}

//-----------------------------------------------------------------------------
bool ASlashCharacter::HasEnoughStamina() const
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

//-----------------------------------------------------------------------------
float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	UpdateHUD();
	return DamageAmount;
}

//-----------------------------------------------------------------------------
void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

//-----------------------------------------------------------------------------
void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && Soul)
	{
		Attributes->AddSouls(Soul->GetSouls());
		UpdateHUD();
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && Treasure)
	{
		Attributes->AddGold(Treasure->GetGold());
		UpdateHUD();
	}
}

void ASlashCharacter::AddHealth(AHealthPickup* HealthPickup)
{
	if (Attributes && HealthPickup)
	{
		Attributes->AddHealth(HealthPickup->GetHealth());
		UpdateHUD();
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	if ((Controller != nullptr) && (Value != 0.f))
	{
		// Find out which way is forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	if ((Controller != nullptr) && (Value != 0.f))
	{
		// Find out which way is right
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value*5.f);
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

//-----------------------------------------------------------------------------
void ASlashCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

//-----------------------------------------------------------------------------
void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

//-----------------------------------------------------------------------------
void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* PlayerCOntroller = Cast<APlayerController>(GetController());
	if (PlayerCOntroller)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerCOntroller->GetHUD());
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
		}
	}

	UpdateHUD();
}

//-----------------------------------------------------------------------------
void ASlashCharacter::UpdateHUD()
{
	if (SlashOverlay == nullptr) return;

	SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	SlashOverlay->SetGoldCount(Attributes->GetGold());
	SlashOverlay->SetSoulsCount(Attributes->GetSouls());
}

//-----------------------------------------------------------------------------
void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

//-----------------------------------------------------------------------------
void ASlashCharacter::EquipWeapon(AWeapon*& OverlappingWeapon)
{
	OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	EquippedWeapon = OverlappingWeapon;
	SetOverlappingItem(nullptr);
}

//-----------------------------------------------------------------------------
bool ASlashCharacter::CanAttack() const
{
	return IsUnocupied() &&	CharacterState != ECharacterState::ECS_Unequipped;
}

//-----------------------------------------------------------------------------
bool ASlashCharacter::CanDisarm() const
{
	return IsUnocupied() &&	CharacterState != ECharacterState::ECS_Unequipped;
}

//-----------------------------------------------------------------------------
bool ASlashCharacter::CanArm() const
{
	return IsUnocupied() &&
		CharacterState == ECharacterState::ECS_Unequipped && 
		EquippedWeapon;
}

//-----------------------------------------------------------------------------
void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

//-----------------------------------------------------------------------------
void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

//-----------------------------------------------------------------------------
void ASlashCharacter::Attack()
{
	Super::Attack();
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::AttackHard()
{
	if (CanAttack())
	{
		PlayRandomMontageSection(AttackMontage, AttackComboMontageSections);
		ActionState = EActionState::EAS_Attacking;
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina()) return;

	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		UpdateHUD();
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
//	SetPlayerAttaker(nullptr);
	UpdateHUD();
}

//-----------------------------------------------------------------------------
void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

//-----------------------------------------------------------------------------
void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}
