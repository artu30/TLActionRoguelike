#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleEmitter.h"
#include "SProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	UParticleSystem* ImpactVFX;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectComp;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float TimeToDetonation = 2.f;

	FTimerHandle TimerHandleDetonation;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnDetonateProjectile();

	virtual void SpawnImpactVFX();

	virtual void OnDestroyProjectile() { Destroy(); };

	UFUNCTION()
	void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
