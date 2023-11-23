#include "SPowerupHealthPotion.h"

#include "SAttributeComponent.h"
#include "SPlayerState.h"
#include "GameFramework/Pawn.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

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

	ApplyCoinsCost(InstigatorPawn, -CreditsCostAmount);

	HidePoweup();
}

FText ASPowerupHealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if (AttributeComp && AttributeComp->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHeathWarning", "Already at full health.");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), CreditsCostAmount);
}

#undef LOCTEXT_NAMESPACE