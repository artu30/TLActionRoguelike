#include "SPowerup.h"
#include "Components/SphereComponent.h"

// Sets default values
ASPowerup::ASPowerup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName(TEXT("Powerup"));
	RootComponent = SphereComp;
}

void ASPowerup::ShowPowerup()
{
	SetPowerupState(true);
}

void ASPowerup::HidePoweup()
{
	SetPowerupState(false);

	GetWorldTimerManager().SetTimer(TimerHandleShowPowerup, this, &ASPowerup::ShowPowerup, RespawnTime);
}

void ASPowerup::SetPowerupState(bool bActive)
{
	SetActorEnableCollision(bActive);

	RootComponent->SetVisibility(bActive, true);
}

void ASPowerup::Interact_Implementation(APawn* InstigatorPawn)
{
	
}