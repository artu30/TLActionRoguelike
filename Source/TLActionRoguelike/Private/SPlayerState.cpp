#include "SPlayerState.h"

void ASPlayerState::ApplyCoinsChange(AActor* InstigatorActor, int32 Delta)
{
	NumCredits = FMath::Clamp(NumCredits + Delta, 0.f, MaxCredits);
	
	OnCoinsChanged.Broadcast(InstigatorActor, NumCredits, Delta);
}