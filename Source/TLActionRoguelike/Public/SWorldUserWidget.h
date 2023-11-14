#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

class USizeBox;

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	AActor* AttachedActor;

protected:
	
	UPROPERTY(meta = (BindWidget))
	USizeBox* ParentSizeBox;

	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
