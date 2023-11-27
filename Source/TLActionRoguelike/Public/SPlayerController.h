#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget* PauseMenuInstance;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerStateChanged;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_PauseMenu;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_AnyKey;

	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	void SetupInputComponent() override;

	void AnyKeyInput();

public:

	UFUNCTION(BlueprintPure, Category= "Input")
	bool IsUsingGamepad() const
	{
		return bIsUsingGamepad;
	}

private:

	/* Was any input recently using GamePad */
	bool bIsUsingGamepad;
	
};
