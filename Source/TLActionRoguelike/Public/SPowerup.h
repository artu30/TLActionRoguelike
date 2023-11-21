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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PowerupMesh;

	UPROPERTY(EditAnywhere, Category = "Powerup")
	float RespawnTime = 10.f;

	UPROPERTY(EditAnywhere, Category = "Powerup")
	int32 CreditsCostAmount = 0;

	FTimerHandle TimerHandleShowPowerup;

	UPROPERTY(ReplicatedUsing = "OnRep_IsActive")
	bool bIsActive = true;
	
	void ShowPowerup();

	void HidePoweup();

	void SetPowerupState(bool bActive);

	UFUNCTION()
	void OnRep_IsActive();

public:

	bool CanInteractPowerup(APawn* InstigatorPawn);
	
	void Interact_Implementation(APawn* InstigatorPawn);

	bool CanTakePowerup(APawn* InstigatorPawn);

	void ApplyCoinsCost(APawn* InstigatorPawn);

};