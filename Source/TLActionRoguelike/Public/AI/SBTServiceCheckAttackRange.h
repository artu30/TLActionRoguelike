#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTServiceCheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API USBTServiceCheckAttackRange : public UBTService
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector WithinAttackRangeKeyName;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float MinDistanceToAttackRange = 2000.f;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
