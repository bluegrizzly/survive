#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class UAnimMontage;
class USoundBase;
class UParticleSystem;
class UHealthBarComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	// Actor
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

	// IHitInterface
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	// AActor
	virtual void BeginPlay() override;

	//ABaseCharacter
	virtual void Die_Implementation() override;
	void SpawnSoul();
	virtual void Attack() override;
	void SetPlayerAttaker(AActor* Attacker);
	virtual bool CanAttack() const override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;

	bool InTargetRange(AActor* Target, double Radius) const;
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 6.f;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	TObjectPtr<AActor> PlayerChasing;

private:
	void SpawnDefaultWeapon();
	void InitializeEnemy();

	// AI Behaviour
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealtBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius() const;
	bool IsOutsideAttackRadius() const;
	bool IsInsideAttackRadius() const;
	bool IsChasing() const;
	bool IsAttacking() const;
	bool IsDead() const;
	bool IsEngaged() const;
	void ClearPatrolTimer();

	// Combat
	void StartAttackTimer();
	void ClearAttackTimer();

	TObjectPtr<class UPawnSensingComponent> PawnSensing;

	UPROPERTY(EditAnywhere, Category = HUD)
	TObjectPtr<UHealthBarComponent> HealthBarWidget;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	double AttackRadius = 150.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	double AcceptanceRadius = 50.f;

	// Navigation
	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;

	FTimerHandle PatrolTimer;

	UPROPERTY(VisibleInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget; //current

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray< TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrolWaitMin = 5.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrolWaitMax = 10.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 350.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> SoulClass;
};
