#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttackComponent.generated.h"

class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TLACTIONROGUELIKE_API USAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	USAttackComponent();

protected:
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> MagicProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> BlackholeProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> TeleportProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* MagicProjectileAttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* BlackholeProjectileAttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* TeleportProjectileAttackAnim;

	FTimerHandle TimerHandlePrimaryAttack;
	FTimerHandle TimerHandleBlackholeAttack;
	FTimerHandle TimerHandleTeleportAttack;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrimaryAttack(UWorld* World);

	void BlackholeAttack(UWorld* World);

	void TeleportAttack(UWorld* World);

	UAnimMontage* GetMagicProjectileAttackAnim() const { return MagicProjectileAttackAnim; }

	UAnimMontage* GetBlackholeProjectileAttackAnim() const { return BlackholeProjectileAttackAnim; }

	UAnimMontage* GetTeleportProjectileAttackAnim() const { return TeleportProjectileAttackAnim; }

protected:

	void AttackFromHand_TimeElapsed(TSubclassOf<AActor> ProjectileClass, UWorld* World);
	
};
