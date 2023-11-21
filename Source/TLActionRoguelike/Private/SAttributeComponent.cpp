#include "SAttributeComponent.h"

#include "SGameModeBase.h"
#include "SPlayerState.h"
#include "AI/SAICharacter.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Health = MaxHealth;
	Rage = MaxRage;

	SetIsReplicatedByDefault(true);
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (!FromActor)
	{
		return nullptr;
	}

	return FromActor->FindComponentByClass<USAttributeComponent>();
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	const USAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (!AttributeComp)
	{
		return false;
	}

	return AttributeComp->IsAlive();
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void USAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.f)
	{
		return false;
	}

	if (Delta < 0.f)
	{
		const float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}
	
	const float OldHealth = Health;
	
	Health = FMath::Clamp(Health + Delta, 0.f, MaxHealth);

	const float CurrentDelta = Health - OldHealth;
	
	//OnHealthChanged.Broadcast(InstigatorActor, this, Health, CurrentDelta);
	if (CurrentDelta != 0.f)
	{
		MulticastHealthChanged(InstigatorActor, Health, CurrentDelta);
	}
	
	if (Delta < 0.f)
	{
		ApplyRageChange(InstigatorActor, -Delta * RageByDamageMultiplier);
	}
	
	// Died
	if (CurrentDelta < 0.f && Health == 0.f)
	{
		ASGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (GameMode)
		{
			GameMode->OnActorKilled(GetOwner(), InstigatorActor);
		}

		APawn* InstigatorPawn = Cast<APawn>(InstigatorActor);
		if (InstigatorPawn)
		{
			ASAICharacter* AIVictim = Cast<ASAICharacter>(GetOwner());
			ASPlayerState* InstigatorPLayerState = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState());
			if (AIVictim && InstigatorPLayerState)
			{
				InstigatorPLayerState->ApplyCoinsChange(AIVictim, CoinsOnDeathAmount);
			}
		}
	}
	
	return CurrentDelta != 0.f;
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	const float OldRage = Rage;
	
	Rage = FMath::Clamp(Rage + Delta, 0.f, MaxRage);

	const float CurrentDelta = Rage - OldRage;

	//OnRageChanged.Broadcast(InstigatorActor, this, Rage, CurrentDelta);
	if (CurrentDelta != 0.f)
	{
		MulticastRageChanged(InstigatorActor, Rage, CurrentDelta);
	}
	
	return CurrentDelta != 0.f;
}

bool USAttributeComponent::FullHeal()
{
	const float CurrentHealth = Health;
	
	Health = MaxHealth;

	const float CurrentDelta = Health - CurrentHealth;
	
	OnHealthChanged.Broadcast(nullptr, this, Health, CurrentDelta);

	return CurrentDelta != 0.f;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, MaxHealth);

	DOREPLIFETIME(USAttributeComponent, Rage);
	DOREPLIFETIME(USAttributeComponent, MaxRage);

	//DOREPLIFETIME_CONDITION(USAttributeComponent, MaxHealth, COND_OwnerOnly);
}