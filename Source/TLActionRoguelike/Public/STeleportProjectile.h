#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "STeleportProjectile.generated.h"

class UParticleSystemComponent;

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API ASTeleportProjectile : public ASProjectileBase
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	ASTeleportProjectile();

protected:
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	float TimeToTeleport = 0.2f;

	FTimerHandle TimerHandleTeleport;

	bool bExploding = false;

	virtual void OnDestroyProjectile() override;
	
	void TeleportPlayer();

};
