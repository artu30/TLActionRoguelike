#include "SPlayerState.h"

#include "SSaveGame.h"
#include "Net/UnrealNetwork.h"

//void ASPlayerState::MulticastCreditsChanged_Implementation(float NewCredits, float Delta)
//{
//	OnCoinsChanged.Broadcast(this, NewCredits, Delta);
//}

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCoinsChanged.Broadcast(this, Credits, Credits - OldCredits);
}

void ASPlayerState::ApplyCoinsChange(AActor* InstigatorActor, int32 Delta)
{
	int32 OldCredits = Credits;
	
	if (Delta > 0.f)
	{
		Credits += Delta;
	}
	else if (Delta < 0.f && (Credits - Delta > 0))
	{
		Credits -= Delta;
	}

	OnRep_Credits(OldCredits);
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (!SaveObject)
	{
		return;
	}

	SaveObject->Credits = Credits;
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (!SaveObject)
	{
		return;
	}

	Credits = SaveObject->Credits;
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}
