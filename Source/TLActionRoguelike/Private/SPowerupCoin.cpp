#include "SPowerupCoin.h"

#include "SPlayerState.h"

void ASPowerupCoin::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!CanInteractPowerup(InstigatorPawn))
	{
		return;
	}
	
	ApplyCoinsCost(InstigatorPawn);
	
	HidePoweup();
}
