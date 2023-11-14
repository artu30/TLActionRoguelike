#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Health = MaxHealth;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (!FromActor)
	{
		return nullptr;
	}

	return FromActor->FindComponentByClass<USAttributeComponent>();
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	const USAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (!AttributeComp)
	{
		return false;
	}

	return AttributeComp->IsAlive();
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	const float CurrentHealth = Health;
	
	Health = FMath::Clamp(Health + Delta, 0.f, MaxHealth);

	const float CurrentDelta = Health - CurrentHealth;
	
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, Delta);

	return CurrentDelta != 0.f;
}

bool USAttributeComponent::FullHeal()
{
	const float CurrentHealth = Health;
	
	Health = MaxHealth;

	const float CurrentDelta = Health - CurrentHealth;
	
	OnHealthChanged.Broadcast(nullptr, this, Health, CurrentDelta);

	return CurrentDelta != 0.f;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}