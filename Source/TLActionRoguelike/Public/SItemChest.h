#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class TLACTIONROGUELIKE_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	ASItemChest();

protected:
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;

	UPROPERTY(EditAnywhere)
	float TargetPitch = 110.f;

	UPROPERTY(ReplicatedUsing = "OnRep_LidOpened", BlueprintReadOnly)
	bool bLidOpened;

	UFUNCTION()
	void OnRep_LidOpened();

public:

	void Interact_Implementation(APawn* InstigatorPawn);
	
};
