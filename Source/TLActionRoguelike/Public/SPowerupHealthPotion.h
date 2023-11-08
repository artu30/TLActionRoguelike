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
	
	// Sets default values for this actor's properties
	ASPowerupHealthPotion();
	
	void Interact_Implementation(APawn* InstigatorPawn);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* HealthPotionMesh;

	UPROPERTY(EditAnywhere, Category = "PowerupHealthPotion")
	float HealthAmount = 50.f;
	
};
