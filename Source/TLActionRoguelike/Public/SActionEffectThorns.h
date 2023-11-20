#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SActionEffectThorns.generated.h"

class USAttributeComponent;

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API USActionEffectThorns : public USActionEffect
{
	GENERATED_BODY()

public:

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override;
	
protected:

	UPROPERTY(EditAnywhere, Category = "Thorns")
	float FractionDamageAmount = 0.2f;

	UFUNCTION()
	void OnOwnerHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);
	
};
