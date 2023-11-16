#include "SGameplayFunctionLibrary.h"

#include "SAttributeComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/HitResult.h"

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(TargetActor);
	if (!AttributeComp)
	{
		return false;
	}

	return AttributeComp->ApplyHealthChange(DamageCauser, DamageAmount);
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	if (!ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		return false;
	}

	UPrimitiveComponent* HitComp = HitResult.GetComponent();
	if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
	{
		// Direction = Target - Origin
		FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
		Direction.Normalize();
		
		HitComp->AddImpulseAtLocation(Direction * 300000.f, HitResult.ImpactPoint, HitResult.BoneName);
	}
	
	return true;
}