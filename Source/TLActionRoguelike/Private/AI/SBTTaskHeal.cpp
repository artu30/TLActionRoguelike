#include "AI/SBTTaskHeal.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type USBTTaskHeal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* Pawn = Cast<ACharacter>(AIController->GetPawn());
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(Pawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if (!AttributeComp)
	{
		return EBTNodeResult::Failed;
	}

	return AttributeComp->FullHeal() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}