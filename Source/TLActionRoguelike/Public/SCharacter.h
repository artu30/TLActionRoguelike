#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USInteractionComponent;
class USAttackComponent;
class USAttributeComponent;
class USActionComponent;

UCLASS()
class TLACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	ASCharacter();

protected:
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere)
	USAttackComponent* AttackComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SProjectile")
	UParticleSystem* CastSpellHandleVFX;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void PrimaryAttack();
	void BlackholeAttack();
	void TeleportAttack();

	void PrimaryInteract();

	void SprintStart();

	void SprintStop();

	UFUNCTION()
	void OnCharacterHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	virtual FVector GetPawnViewLocation() const override;
	
public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector GetAttackStartLocation() const;

	FVector GetAttackEndLocation() const;

	FRotator GetAttackStartRotation() const;

	UFUNCTION(BlueprintCallable)
	USAttributeComponent* GetAttributeComponent() const;

	void SpawnCastSpellHandVFX();

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100.f);

};
