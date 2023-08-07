
#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Engine/World.h"
#include "Items/Treasure.h"
#include "Items/HealthPickup.h"
#include "Components/CapsuleComponent.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"

//-----------------------------------------------------------------------------
ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection"));
	SetRootComponent(GeometryCollection.Get());
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

//-----------------------------------------------------------------------------
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnBreakEvent);
}

//-----------------------------------------------------------------------------
void ABreakableActor::OnBreakEvent(const FChaosBreakEvent& BreakEvent)
{
}

//-----------------------------------------------------------------------------
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//-----------------------------------------------------------------------------
void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken || GetWorld() == nullptr) return;

	bBroken = true;
	FVector Location = GetActorLocation();
	Location.Z += 75.f;
	if (TreasureClass.Num() > 0)
	{
		const int32 TreasureIndex = FMath::RandRange(0, TreasureClass.Num()-1);
		GetWorld()->SpawnActor<ATreasure>(TreasureClass[TreasureIndex], Location, GetActorRotation());
	}
	if (HealthClass.Num() > 0)
	{
		const int32 HealthIndex = FMath::RandRange(0, HealthClass.Num() - 1);
		GetWorld()->SpawnActor<AHealthPickup>(HealthClass[HealthIndex], Location, GetActorRotation());
	}
}
