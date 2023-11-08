#include "SBlackholeProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SphereComponent.h"

ASBlackholeProjectile::ASBlackholeProjectile()
{
	MovementComp->InitialSpeed = 200.f;
	
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->Radius = 500.f;
	RadialForceComp->ImpulseStrength = -500.f;
	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
	RadialForceComp->SetupAttachment(SphereComp);
}