#include "SPowerupHealthPotion.h"

#include "SAttributeComponent.h"
#include "Components/StaticMeshComponent.h"

ASPowerupHealthPotion::ASPowerupHealthPotion()
{
	HealthPotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HealthPotionMesh"));
	HealthPotionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthPotionMesh->SetupAttachment(RootComponent);
}

void ASPowerupHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!InstigatorPawn)
	{
		return;
	}

	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if (!AttributeComp)
	{
		return;
	}

	if (AttributeComp->IsFullHealth())
	{
		return;
	}

	if (!AttributeComp->ApplyHealthChange(HealthAmount))
	{
		return;
	}

	HidePoweup();
}