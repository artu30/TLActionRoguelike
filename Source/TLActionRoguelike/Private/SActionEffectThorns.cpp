#include "SActionEffectThorns.h"

#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SGameplayFunctionLibrary.h"

void USActionEffectThorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if (!AttributeComp)
	{
		return;
	}

	AttributeComp->OnHealthChanged.AddDynamic(this, &USActionEffectThorns::OnOwnerHealthChanged);
}

void USActionEffectThorns::StopAction_Implementation(AActor* Instigator)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if (!AttributeComp)
	{
		return Super::StopAction_Implementation(Instigator);
	}

	AttributeComp->OnHealthChanged.RemoveDynamic(this, &USActionEffectThorns::OnOwnerHealthChanged);

	Super::StopAction_Implementation(Instigator);
}

void USActionEffectThorns::OnOwnerHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	const AActor* OwningActor = GetOwningComponent()->GetOwner();
	
	if (Delta >= 0.f || InstigatorActor == OwningActor)
	{
		return;	
	}

	int32 ReflectedAmount = FMath::RoundToInt(Delta * FractionDamageAmount);
	if (ReflectedAmount == 0)
	{
		return;
	}

	USGameplayFunctionLibrary::ApplyDamage(GetOwningComponent()->GetOwner(), InstigatorActor, ReflectedAmount);
}