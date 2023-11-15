#pragma once

#include "CoreMinimal.h"
#include "SPowerup.h"
#include "SPowerupHealthPotion.generated.h"

class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API ASPowerupHealthPotion : public ASPowerup
{
	GENERATED_BODY()

public:
	
	void Interact_Implementation(APawn* InstigatorPawn);

protected:

	UPROPERTY(EditAnywhere, Category = "PowerupHealthPotion")
	float HealthAmount = 50.f;

};
