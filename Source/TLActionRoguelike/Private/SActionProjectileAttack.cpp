#include "SActionProjectileAttack.h"

#include "SAttributeComponent.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

USActionProjectileAttack::USActionProjectileAttack()
{
	
}

void USActionProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* CharacterInstigator = Cast<ACharacter>(Instigator);
	if (!CharacterInstigator)
	{
		return;
	}
	
	CharacterInstigator->PlayAnimMontage(AttackAnim);

	UGameplayStatics::SpawnEmitterAttached(CastingEffect, CharacterInstigator->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

	FTimerHandle TimerHandleAttackDelay;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "AttackDelayElapsed", CharacterInstigator);

	GetWorld()->GetTimerManager().SetTimer(TimerHandleAttackDelay, Delegate, AttackAnimDelay, false);
}

bool USActionProjectileAttack::CanStart_Implementation(AActor* Instigator)
{
	if (!Super::CanStart_Implementation(Instigator))
	{
		return false;
	}
	
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Instigator);
	if (!ensure(AttributeComp))
	{
		return false;
	}

	if (!AttributeComp->HasEnoughRage(RageCost))
	{
		return false;
	}

	return true;
}

void USActionProjectileAttack::AttackDelayElapsed(ACharacter* InstigatorCharacter)
{
	if (!ensure(ProjectileClass))
	{
		return;
	}

	FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = InstigatorCharacter;

	FCollisionShape Shape;
	Shape.SetSphere(20.f);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(InstigatorCharacter);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();

	FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * 5000.f);

	FHitResult Hit;
	if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params))
	{
		TraceEnd = Hit.ImpactPoint;
	}
	
	FRotator ProjectileRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();
	
	FTransform SpawnTM = FTransform(ProjectileRotation, HandLocation);

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

	StopAction(InstigatorCharacter);
}