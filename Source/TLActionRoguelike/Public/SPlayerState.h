#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCoinsChanged, AActor*, InstigatorActor, int32, NewCoins, int32, DeltaCoins);

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	int32 NumCredits = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	int32 MaxCredits = 100;
	
public:

	UPROPERTY(BlueprintAssignable)
	FOnCoinsChanged OnCoinsChanged;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void ApplyCoinsChange(AActor* InstigatorActor, int32 Delta);

	int32 GetNumCredits() const { return NumCredits; }
	
};
