#pragma once

#include "CoreMinimal.h"
#include "SPowerup.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class USSaveGame;
class UDataTable;
class USMonsterData;

USTRUCT(BlueprintType)
struct FSPowerupData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int32 NumberOfPowerupsToSpawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASPowerup> PowerupClass;
};

/* DataTable Row for spawning monster in game mode */
USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

	FMonsterInfoRow()
	{
		Weight = 1.f;
		SpawnCost = 5.f;
		KillReward = 20.f;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId;
	//TSubclassOf<AActor> MonsterClass;
	
	/* Relative chance to pick this monster. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	/* Points required by gamemode to spawn this unit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* Amount of credits awarded to killer of this unit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
	
};

/**
 * 
 */
UCLASS()
class TLACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FTimerHandle TimerHandleSpawnBots;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (EditCondition = "DifficultyCurve == nullptr"))
	float MaxAliveBots = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float TimeToRespawn = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	TArray<FSPowerupData> PowerupClasses;
	
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	UEnvQuery* SpawnPowerupEQS;

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

	FString SlotName = "SaveGame01";

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* MonsterTable;

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnSpawnPowerupsQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	UFUNCTION()
	void SpawnBotTimerElapsed();

	void SpawnPowerups();

	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation, float KillReward);
	
public:

	ASGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAllAI();

	virtual void OnActorKilled(AActor* VictimActor, AActor* KillerActor);

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();
	
};
