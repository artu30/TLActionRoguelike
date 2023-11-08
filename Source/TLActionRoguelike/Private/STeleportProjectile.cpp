#include "STeleportProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"

ASTeleportProjectile::ASTeleportProjectile()
{
	MovementComp->InitialSpeed = 1200.f;
}

void ASTeleportProjectile::OnDestroyProjectile()
{
	if (bExploding)
	{
		return;
	}

	bExploding = true;
	
	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	EffectComp->DeactivateSystem();
	
	GetWorldTimerManager().SetTimer(TimerHandleTeleport, this, &ASTeleportProjectile::TeleportPlayer, TimeToTeleport, false);
}

void ASTeleportProjectile::TeleportPlayer()
{
	if (!GetInstigator())
	{
		return;
	}
	
	GetInstigator()->TeleportTo(GetActorLocation(), GetActorRotation());

	SetLifeSpan(0.2f);
}