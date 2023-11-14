#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class URadialForceComponent;

UCLASS()
class TLACTIONROGUELIKE_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();
	
protected:
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ExplosiveBarrelMesh;

	UPROPERTY(VisibleAnywhere)
	URadialForceComponent* RadialForceComp;

	UPROPERTY(EditAnywhere, Category = "ExplosiveBarrel")
	float HitDamage = -50.f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHitBarrel(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Explode(AActor* OtherActor);
	
};
