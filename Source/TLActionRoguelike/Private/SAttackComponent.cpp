#include "SAttackComponent.h"
#include "SCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
USAttackComponent::USAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void USAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void USAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USAttackComponent::PrimaryAttack(UWorld* World)
{
	FTimerDelegate PrimaryAttackDelegate = FTimerDelegate::CreateUObject(this, &USAttackComponent::AttackFromHand_TimeElapsed, MagicProjectileClass, World);
	World->GetTimerManager().SetTimer(TimerHandlePrimaryAttack, PrimaryAttackDelegate, 0.2f, false);
}

void USAttackComponent::BlackholeAttack(UWorld* World)
{
	FTimerDelegate BlackholeAttackDelegate = FTimerDelegate::CreateUObject(this, &USAttackComponent::AttackFromHand_TimeElapsed, BlackholeProjectileClass, World);
	World->GetTimerManager().SetTimer(TimerHandleBlackholeAttack, BlackholeAttackDelegate, 0.2f, false);
}

void USAttackComponent::TeleportAttack(UWorld* World)
{
	FTimerDelegate TeleportAttackDelegate = FTimerDelegate::CreateUObject(this, &USAttackComponent::AttackFromHand_TimeElapsed, TeleportProjectileClass, World);
	World->GetTimerManager().SetTimer(TimerHandleTeleportAttack, TeleportAttackDelegate, 0.2f, false);
}

void USAttackComponent::AttackFromHand_TimeElapsed(TSubclassOf<AActor> ProjectileClass, UWorld* World)
{
	ensure(ProjectileClass);
	
	ASCharacter* CharacterOwner = Cast<ASCharacter>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}

	FRotator ProjectileRotation = FRotationMatrix::MakeFromX(CharacterOwner->GetAttackEndLocation() - CharacterOwner->GetAttackStartLocation()).Rotator();
	
	FTransform SpawnTM = FTransform(ProjectileRotation, CharacterOwner->GetAttackStartLocation());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = CharacterOwner;
	
	World->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}