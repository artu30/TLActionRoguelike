#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SAIController.generated.h"

class UBehaviorTree;

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API ASAIController : public AAIController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;
	
	virtual void BeginPlay() override;
	
};
