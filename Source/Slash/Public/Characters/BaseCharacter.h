#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	void SetAttackingEnemy(AActor* Enemy);

	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }

protected:
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();
	virtual void AttackHard();
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	UFUNCTION(BlueprintNativeEvent)
	void Die();

	// Play Montage functions
	void StopAttackMontage();
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWrapTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWrapTarget();

	virtual void PlayEquipMontage(const FName& SectionName);
	virtual void PlayHitReactMontage(const FName& SectionName = FName());
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void DisableCapsule();
	void DisableMeshCollision();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual bool CanAttack() const;
	virtual bool CanDisarm() const;
	virtual bool CanArm() const;
	bool IsAlive() const; 

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAttributeComponent> Attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr <class AWeapon> EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr <UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr <UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr <UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr <UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr <UAnimMontage> DodgeMontage;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;

private:
	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr <class USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr <class UParticleSystem> HitParticle;
};
