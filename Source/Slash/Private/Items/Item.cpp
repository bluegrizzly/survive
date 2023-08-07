#include "Items/Item.h"
#include "Slash/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

//-----------------------------------------------------------------------------
AItem::AItem() : 
	 Amplitude(0.25f) 
	,TimeConstant(5.f)
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Ember VFX"));
	ItemEffect->SetupAttachment(GetRootComponent());
}

//-----------------------------------------------------------------------------
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	//FVector location = GetActorLocation();
	//FVector forward = GetActorForwardVector();

	//DRAW_SPHERE(location, FColor::Red)
	//DRAW_VECTOR(location, location + forward * 100.f, FColor::Red)

	// Bind delegate
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

//-----------------------------------------------------------------------------
void AItem::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	runningTime += deltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformSin()));
	}


	//float deltaZ = TransformSin(runningTime);
	//AddActorWorldOffset(FVector(0.f, 0.f, deltaZ));

	//// Movement rate in units of cm/s
	//float movementRate = 50.f;
	//float rotationRate = 45.f;
	//// Movement rate * deltatime (cm/s) *  (s/frame) = (cm/frame)
	//AddActorWorldOffset(FVector(movementRate * deltaTime, 0.f, 0.f));
	//AddActorWorldRotation(FRotator(0.f, rotationRate * deltaTime, 0.f));


	//DRAW_SPHERE_SingleFrame(GetActorLocation(), FColor::Red, 25.f);
	//DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f, FColor::Red);
}

//-----------------------------------------------------------------------------
float AItem::TransformSin()
{
	return Amplitude * FMath::Sin(runningTime * TimeConstant);
}

//-----------------------------------------------------------------------------
float AItem::TransformCos()
{
	return Amplitude * FMath::Cos(runningTime * TimeConstant);
}

//-----------------------------------------------------------------------------
void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface /*&& SlashCharacter->GetCharacterState() == ECharacterState::ECS_Unequipped*/)
	{
		PickupInterface->SetOverlappingItem(this);
	}
}

//-----------------------------------------------------------------------------
void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingItem(nullptr);
	}
}

//-----------------------------------------------------------------------------
void AItem::SpawnPickupSystem()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation()
		);
	}
}

void AItem::SpawnPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}
}

