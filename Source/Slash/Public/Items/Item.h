#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	float TransformSin();
	UFUNCTION(BlueprintPure)
	float TransformCos();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sin Parameters")
	float Amplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sin Parameters")
	float TimeConstant;

	template<typename T> 
	T Avg(T first, T second);

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void SpawnPickupSystem();
	UFUNCTION()
	void SpawnPickupSound();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr <UStaticMeshComponent> ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr <USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere, Category = VFX)
	TObjectPtr<class UNiagaraComponent> ItemEffect;

	UPROPERTY(EditAnywhere, Category = VFX)
	TObjectPtr<class UNiagaraSystem> PickupEffect;

	UPROPERTY(EditAnywhere, Category = VFX)
	USoundBase* PickupSound;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float runningTime;
};

template<typename T>
inline T AItem::Avg(T first, T second)
{
	return (first + second) / 2;
}
