#pragma once

#include "CoreMinimal.h"
#include "SPowerup.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class USSaveGame;

USTRUCT(BlueprintType)
struct FSPowerupData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int32 NumberOfPowerupsToSpawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASPowerup> PowerupClass;
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
	TSubclassOf<AActor> MinionClass;

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

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnSpawnPowerupsQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	UFUNCTION()
	void SpawnBotTimerElapsed();

	void SpawnPowerups();
	
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
