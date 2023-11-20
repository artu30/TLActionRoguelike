#include "SPowerupAction.h"

#include "SActionComponent.h"

void ASPowerupAction::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn && ActionToGrant))
	{
		return;	
	}

	USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	if (!ActionComp)
	{
		return;
	}

	if (ActionComp->GetAction(ActionToGrant))
	{
		FString DebugMsg = FString::Printf(TEXT("Action '%s' already known."), *GetNameSafe(ActionToGrant));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, DebugMsg);
		return;
	}

	ActionComp->AddAction(InstigatorPawn, ActionToGrant);
	HidePoweup();
}
