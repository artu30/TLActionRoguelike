#pragma once

#include "CoreMinimal.h"
#include "SPowerup.h"
#include "SPowerupCoin.generated.h"

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API ASPowerupCoin : public ASPowerup
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "PowerupCoin")
	int32 CreditsAmount = 100;

public:

	void Interact_Implementation(APawn* InstigatorPawn);
	
};
