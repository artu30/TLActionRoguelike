#include "AI/SBTTaskRangeAttack.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type USBTTaskRangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComp))
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	if (!USAttributeComponent::IsActorAlive(TargetActor))
	{
		return EBTNodeResult::Failed;
	}

	const FVector MuzzleLocation = Pawn->GetMesh()->GetSocketLocation("Muzzle_01");

	const FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
	FRotator MuzzleRotation = Direction.Rotation();

	MuzzleRotation.Pitch += FMath::RandRange(0.f, MaxBulletsSpread);
	MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletsSpread, MaxBulletsSpread);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Instigator = Pawn;

	const AActor* NewProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);

	return NewProjectile ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
