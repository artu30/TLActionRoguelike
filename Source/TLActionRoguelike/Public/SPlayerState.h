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

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = "OnRep_Credits", Category = "Credits")
	int32 NumCredits = 50;

	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);

	//UFUNCTION(NetMulticast, Unreliable)
	//void MulticastCreditsChanged(float NewCredits, float Delta);
	
public:

	UPROPERTY(BlueprintAssignable)
	FOnCoinsChanged OnCoinsChanged;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void ApplyCoinsChange(AActor* InstigatorActor, int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetNumCredits() const { return NumCredits; }
	
};
