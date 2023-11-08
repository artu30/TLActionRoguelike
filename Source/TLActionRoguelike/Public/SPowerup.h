#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SPowerup.generated.h"

class USphereComponent;

UCLASS()
class TLACTIONROGUELIKE_API ASPowerup : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	ASPowerup();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, Category = "Powerup")
	float RespawnTime = 10.f;

	FTimerHandle TimerHandleShowPowerup;

	void ShowPowerup();

	void HidePoweup();

	void SetPowerupState(bool bActive);

public:

	void Interact_Implementation(APawn* InstigatorPawn);

};
