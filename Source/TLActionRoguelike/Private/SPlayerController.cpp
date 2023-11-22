#include "SPlayerController.h"

void ASPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}

void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateChanged.Broadcast(PlayerState);
}
