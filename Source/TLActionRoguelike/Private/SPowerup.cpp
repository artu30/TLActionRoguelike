#include "SPowerup.h"

#include "SPlayerState.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerup::ASPowerup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName(TEXT("Powerup"));
	RootComponent = SphereComp;

	PowerupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerupMesh"));
	PowerupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PowerupMesh->SetupAttachment(RootComponent);

	bReplicates = true;
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
	bIsActive = bActive;
	OnRep_IsActive();
}

void ASPowerup::OnRep_IsActive()
{
	SetActorEnableCollision(bIsActive);

	RootComponent->SetVisibility(bIsActive, true);
}

bool ASPowerup::CanInteractPowerup(APawn* InstigatorPawn)
{
	if (!InstigatorPawn)
	{
		return false;
	}

	if (!CanTakePowerup(InstigatorPawn))
	{
		return false;
	}

	return true;
}

void ASPowerup::Interact_Implementation(APawn* InstigatorPawn)
{
	
}

bool ASPowerup::CanTakePowerup(APawn* InstigatorPawn)
{
	if (CreditsCostAmount >= 0)
	{
		return true;
	}
	
	if (!InstigatorPawn)
	{
		return false;
	}
	
	// Apply credits
	ASPlayerState* PlayerState = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState());
	if (!PlayerState)
	{
		return false;
	}

	return FMath::Abs(CreditsCostAmount) <= PlayerState->GetNumCredits();
}

void ASPowerup::ApplyCoinsCost(APawn* InstigatorPawn)
{
	if (!InstigatorPawn)
	{
		return;
	}
	
	// Apply credits
	ASPlayerState* PlayerState = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState());
	if (!PlayerState)
	{
		return;
	}

	PlayerState->ApplyCoinsChange(this, CreditsCostAmount);
}

void ASPowerup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerup, bIsActive);
}