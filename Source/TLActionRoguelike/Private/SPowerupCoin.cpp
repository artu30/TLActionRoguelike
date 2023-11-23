#include "SPowerupCoin.h"

void ASPowerupCoin::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!CanInteractPowerup(InstigatorPawn))
	{
		return;
	}
	
	ApplyCoinsCost(InstigatorPawn, CreditsCostAmount);
	
	HidePoweup();
}
