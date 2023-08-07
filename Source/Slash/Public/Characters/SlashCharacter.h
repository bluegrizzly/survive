#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	// IPickupInterface
	virtual void SetOverlappingItem(class AItem* Item) override;
	virtual void AddSouls(class ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Treasure) override;
	virtual void AddHealth(class AHealthPickup* HealthPickup) override;

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	// IHitInterface
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	// AActor
	virtual void BeginPlay() override;

	// Callback for inputs
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	void Dodge();

	void EquipWeapon(AWeapon*& OverlappingWeapon);
	
	// ABaseCharacter
	virtual void Attack() override;
	virtual void AttackHard() override;
	virtual void Die_Implementation() override;

	// Play Montage functions
	virtual void PlayEquipMontage(const FName& SectionName) override;

	// Combat
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() const override;
	virtual bool CanDisarm() const override;
	virtual bool CanArm() const override;
	void Disarm();
	void Arm();

	bool IsUnocupied() const;
	bool IsOccupied() const;
	bool HasEnoughStamina() const;
	void UpdateHUD();

	// Blueprint callable
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();
	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> AttackComboMontageSections;

private:
	void InitializeSlashOverlay();

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<class UGroomComponent> Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<class UGroomComponent> Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr <class AItem> OverlappingItem;

	TObjectPtr <class USlashOverlay> SlashOverlay;
};
