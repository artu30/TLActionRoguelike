#include "SPowerupHealthPotion.h"

#include "SAttributeComponent.h"
#include "SPlayerState.h"

void ASPowerupHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!CanInteractPowerup(InstigatorPawn))
	{
		return;
	}
	
	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if (!AttributeComp)
	{
		return;
	}

	if (AttributeComp->IsFullHealth())
	{
		return;
	}
	
	if (!AttributeComp->ApplyHealthChange(this, HealthAmount))
	{
		return;
	}

	// Apply credits
	ASPlayerState* PlayerState = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState());
	if (!PlayerState)
	{
		return;
	}

	ApplyCoinsCost(InstigatorPawn);

	HidePoweup();
}
