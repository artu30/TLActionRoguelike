#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandleSpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

	SpawnPowerups();
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (!ensure(Controller))
	{
		return;
	}

	Controller->UnPossess();
	
	RestartPlayer(Controller);
}

void ASGameModeBase::SpawnPowerups()
{
	if (PowerupClasses.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No powerups assigned to spawn."));
		return;
	}
	
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnPowerupEQS, this, EEnvQueryRunMode::AllMatching, nullptr);

	QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnPowerupsQueryCompleted);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}
	
	int32 NumOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NumOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NumOfAliveBots);

	float MaxBotsByDifficulty = MaxAliveBots;
	if (DifficultyCurve)
	{
		MaxBotsByDifficulty = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	
	if (NumOfAliveBots >= MaxBotsByDifficulty)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max num of bots reached!"))
		return;
	}
	
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!ensure(QueryInstance))
	{
		return;
	}

	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"))
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsEmpty())
	{
		return;
	}

	GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

	DrawDebugSphere(GetWorld(), Locations[0], 50.f, 20, FColor::Blue, false, 60.f);
}

void ASGameModeBase::OnSpawnPowerupsQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!ensure(QueryInstance))
	{
		return;
	}

	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn health potion EQS Query Failed!"))
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	bool HasEnoughLocations = true;
	for (FSPowerupData PowerUp : PowerupClasses)
	{
		for (int32 Index = 0; Index < PowerUp.NumberOfPowerupsToSpawn; ++Index)
		{
			if (Locations.IsEmpty())
			{
				HasEnoughLocations = false;
				break;
			}

			int32 RandomIndex = FMath::RandRange(0, Locations.Num() - 1);
			FVector LocationToSpawn = Locations[RandomIndex];
			Locations.RemoveAt(RandomIndex);
			
			GetWorld()->SpawnActor<AActor>(PowerUp.PowerupClass, LocationToSpawn, FRotator::ZeroRotator);

			DrawDebugSphere(GetWorld(), LocationToSpawn, 50.f, 20, FColor::Blue, false, 60.f);
		}

		if (!HasEnoughLocations)
		{
			break;
		}
	}
}

void ASGameModeBase::KillAllAI()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); // @fixme: pass in player? for kill credit
		}
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* KillerActor)
{
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (!Player)
	{
		return;
	}

	FTimerHandle TimerHandleRespawn;

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

	GetWorldTimerManager().SetTimer(TimerHandleRespawn, Delegate, TimeToRespawn, false);

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(KillerActor));
}