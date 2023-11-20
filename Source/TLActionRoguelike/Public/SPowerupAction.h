#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SPowerup.h"
#include "SPowerupAction.generated.h"

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API ASPowerupAction : public ASPowerup
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Powerup")
	TSubclassOf<USAction> ActionToGrant;

public:

	void Interact_Implementation(APawn* InstigatorPawn) override;
	
};
