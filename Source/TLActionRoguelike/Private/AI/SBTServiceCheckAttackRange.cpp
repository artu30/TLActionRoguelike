#include "AI/SBTServiceCheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTServiceCheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComp))
	{
		return;
	}

	const AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!TargetActor)
	{
		return;
	}

	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!ensure(AIController))
	{
		return;
	}

	const APawn* AIPawn = AIController->GetPawn();
	if (!ensure(AIPawn))
	{
		return;
	}

	const float SqrDistanceTo = FVector::DistSquared(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

	const bool bWithinRange = SqrDistanceTo < (MinDistanceToAttackRange * MinDistanceToAttackRange);

	bool bHasLOS = false;
	if (bWithinRange)
	{
		bHasLOS = AIController->LineOfSightTo(TargetActor);
	}

	BlackboardComp->SetValueAsBool(WithinAttackRangeKeyName.SelectedKeyName, (bWithinRange && bHasLOS));
}
