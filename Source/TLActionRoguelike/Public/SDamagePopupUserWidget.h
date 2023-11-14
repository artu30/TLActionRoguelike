#pragma once

#include "CoreMinimal.h"
#include "SWorldUserWidget.h"
#include "SDamagePopupUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API USDamagePopupUserWidget : public USWorldUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	float TextToShow;

};
