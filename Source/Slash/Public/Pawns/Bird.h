#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"

class UCapsuleComponent; 
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class SLASH_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void Turn(float Value);
	void LookUp(float Value);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr <UCapsuleComponent> Capsule;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr <USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr <USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr <UCameraComponent> ViewCamera;
};
