#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SActionProjectileAttack.generated.h"

class ACharacter;
class UAnimMontage;
class UParticleSystem;

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API USActionProjectileAttack : public USAction
{
	GENERATED_BODY()

public:

	USActionProjectileAttack();
	
protected:

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName = TEXT("Muzzle_01");

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackAnimDelay = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* CastingEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float RageCost = 0.f;

public:

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual bool CanStart_Implementation(AActor* Instigator) override;

protected:

	UFUNCTION()
	void AttackDelayElapsed(ACharacter* InstigatorCharacter);
	
};
