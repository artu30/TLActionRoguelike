#include "SPlayerState.h"

#include "Net/UnrealNetwork.h"

//void ASPlayerState::MulticastCreditsChanged_Implementation(float NewCredits, float Delta)
//{
//	OnCoinsChanged.Broadcast(this, NewCredits, Delta);
//}

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCoinsChanged.Broadcast(this, NumCredits, NumCredits - OldCredits);
}

void ASPlayerState::ApplyCoinsChange(AActor* InstigatorActor, int32 Delta)
{
	int32 OldCredits = NumCredits;
	
	if (Delta > 0.f)
	{
		NumCredits += Delta;
	}
	else if (Delta < 0.f && (NumCredits - Delta > 0))
	{
		NumCredits -= Delta;
	}

	OnRep_Credits(OldCredits);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, NumCredits);
}
