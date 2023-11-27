#include "SPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

void ASPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}

void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateChanged.Broadcast(PlayerState);
}

void ASPlayerController::TogglePauseMenu()
{
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());

		return;
	}
	
	PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
	if (PauseMenuInstance)
	{
		PauseMenuInstance->AddToViewport(100);

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// New Enhanced Input system
	UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(InputComponent);
	
	InputComp->BindAction(Input_PauseMenu, ETriggerEvent::Triggered, this, &ASPlayerController::TogglePauseMenu);
	InputComp->BindAction(Input_AnyKey, ETriggerEvent::Triggered, this, &ASPlayerController::AnyKeyInput);
	
	/* OLD INPUT
	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ASPlayerController::TogglePauseMenu);

	// Keeping as 'old' input for now until we figure out how to do this easily in Enhanced input
	InputComponent->BindAction("AnyKey", IE_Pressed, this, &ASPlayerController::AnyKeyInput);
	*/
}

void ASPlayerController::AnyKeyInput()
{
	bIsUsingGamepad = false;
}