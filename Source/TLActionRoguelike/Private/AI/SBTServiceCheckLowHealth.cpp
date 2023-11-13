#include "AI/SBTServiceCheckLowHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

void USBTServiceCheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComp))
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

	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(AIPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if (!AttributeComp)
	{
		return;
	}

	BlackboardComp->SetValueAsBool(LowHealthKeyName.SelectedKeyName, AttributeComp->IsLowHealth());
}