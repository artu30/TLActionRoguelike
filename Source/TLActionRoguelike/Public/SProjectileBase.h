#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UAudioComponent;
class USoundCue;
class USoundBase;
class UCameraShakeBase;

UCLASS()
class TLACTIONROGUELIKE_API ASProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectileBase();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SProjectile")
	UParticleSystem* ImpactVFX;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectComp;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComp;

	UPROPERTY(EditAnywhere,  Category = "SProjectile")
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "SProjectile")
	float TimeToDetonation = 2.f;

	UPROPERTY(EditAnywhere, Category = "SProjectile")
	float HitDamage = -20.f;

	UPROPERTY(EditDefaultsOnly, Category = "SProjectile")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditAnywhere, Category = "SProjectile")
	float CameraShakeMinRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "SProjectile")
	float CameraShakeMaxRadius = 2000.f;

	FTimerHandle TimerHandleDetonation;

	virtual void PostInitializeComponents() override;

	void OnDetonateProjectile();

	virtual void SpawnImpactVFX();

	virtual void SpawnImpactSound();

	virtual void OnDestroyProjectile() { Destroy(); };

	UFUNCTION()
	void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
