#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Health = MaxHealth;
}

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	const float CurrentHealth = Health;
	
	Health = FMath::Clamp(Health + Delta, 0.f, MaxHealth);

	const float CurrentDelta = Health - CurrentHealth;
	
	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);

	return CurrentDelta != 0.f;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}