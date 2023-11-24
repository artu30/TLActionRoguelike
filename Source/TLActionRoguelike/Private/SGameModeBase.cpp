#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "TimerManager.h"
#include "AI/SAICharacter.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "TLActionRoguelike/TLActionRoguelike.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	if (SelectedSaveSlot.Len() > 0)
	{
		SlotName = SelectedSaveSlot;
	}

	LoadSaveGame();
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

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation, float KillReward)
{
	UAssetManager* Manager = UAssetManager::GetIfInitialized();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));
		if (MonsterData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);

			if (NewBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				ASAICharacter* NewAICharacter = Cast<ASAICharacter>(NewBot);
				if (NewAICharacter)
				{
					NewAICharacter->SetKillReward(KillReward);
				}

				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<USAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
	}
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

	if (!MonsterTable)
	{
		return;
	}

	TArray<FMonsterInfoRow*> Rows;
	MonsterTable->GetAllRows("", Rows);

	// Get random enemy
	const int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
	FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

	UAssetManager* Manager = UAssetManager::GetIfInitialized();
	if (Manager)
	{
		TArray<FName> Bundles;
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0], SelectedRow->KillReward);
		Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
	}
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

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	ASPlayerState* PlayerState = NewPlayer->GetPlayerState<ASPlayerState>();
	if (PlayerState)
	{
		PlayerState->LoadPlayerState(CurrentSaveGame);
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ASGameModeBase::WriteSaveGame()
{
	const int32 NumPlayers = GameState->PlayerArray.Num();
	for (int32 i = 0; i < NumPlayers; i++)
	{
		ASPlayerState* PlayerState = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PlayerState)
		{
			PlayerState->SavePlayerState(CurrentSaveGame);
			break; // Single player only at this point
		}
	}

	CurrentSaveGame->SavedActors.Empty();
	
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;

		// Only interested in our 'gameplay actors'
		if (!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetActorTransform();

		FMemoryWriter MemWriter(ActorData.ByteData);
		
		FObjectAndNameAsStringProxyArchive Archive(MemWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Archive.ArIsSaveGame = true;
		// Converts Actor's SaveGame UPROPERTIES into binary array
		Actor->Serialize(Archive);
		
		CurrentSaveGame->SavedActors.Add(ActorData);
	}
	
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));

		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;

			// Only interested in our 'gameplay actors'
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);
		
					FObjectAndNameAsStringProxyArchive Archive(MemReader, true);
					Archive.ArIsSaveGame = true;
					// Converts binary array back into actor's variables
					Actor->Serialize(Archive);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);
					
					break;
				}
			}
		}
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created new SaveGame Data."));
	}
}
