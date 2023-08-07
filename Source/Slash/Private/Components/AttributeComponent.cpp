#include "Components/AttributeComponent.h"

//-----------------------------------------------------------------------------
UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

//-----------------------------------------------------------------------------
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

//-----------------------------------------------------------------------------
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//-----------------------------------------------------------------------------
void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

//-----------------------------------------------------------------------------
void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

//-----------------------------------------------------------------------------
float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

//-----------------------------------------------------------------------------
float UAttributeComponent::GetStaminaPercent() const
{
	return Stamina / MaxStamina;
}

//-----------------------------------------------------------------------------
bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

//-----------------------------------------------------------------------------
bool UAttributeComponent::RegenStamina(float DeltaTime)
{
	if (Stamina == MaxStamina) return false;
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
	return true;
}

//-----------------------------------------------------------------------------
void UAttributeComponent::AddHealth(float HealthCount)
{
	Health = FMath::Clamp(Health + HealthCount, 0.f, MaxHealth);
}

//-----------------------------------------------------------------------------
void UAttributeComponent::AddGold(int32 Amount)
{
	Gold += Amount;
}

//-----------------------------------------------------------------------------
void UAttributeComponent::AddSouls(int32 Amount)
{
	Souls += Amount;
}