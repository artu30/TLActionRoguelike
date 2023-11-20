#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewValue, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TLACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static USAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);
	
	// Sets default values for this component's properties
	USAttributeComponent();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float LowHealthAmount = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 CoinsOnDeathAmount = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxRage = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float RageByDamageMultiplier = 1.f;

public:

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnRageChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);

	bool FullHeal();

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	float GetMaxHealth() const { return MaxHealth; }

	bool IsFullHealth() const { return Health >= MaxHealth; }

	bool IsLowHealth() const { return Health <= LowHealthAmount; }

	bool HasEnoughRage(float RageCost) const { return Rage >= RageCost; }

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);
	
};
