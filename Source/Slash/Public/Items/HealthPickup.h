#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "HealthPickup.generated.h"

UCLASS()
class SLASH_API AHealthPickup : public AItem
{
	GENERATED_BODY()

public:
	FORCEINLINE int32 GetHealth() const { return Health; }
	FORCEINLINE void SetSouls(int32 Value) { Health = Value; }
protected:
	// IItem
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Health Properties")
	int32 Health;
};
