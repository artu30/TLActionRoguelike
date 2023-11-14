#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STargetDummy.generated.h"

class USAttributeComponent;
class USDamagePopupUserWidget;

UCLASS()
class TLACTIONROGUELIKE_API ASTargetDummy : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	ASTargetDummy();

protected:

	UPROPERTY(VisibleAnywhere)
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DamagePopUpWidgetClass;
	
	UPROPERTY()
	USDamagePopupUserWidget* DamagePopUp;

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnTargetDummyHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

};
