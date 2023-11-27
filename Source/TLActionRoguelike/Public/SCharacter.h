#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SProjectile")
	UParticleSystem* CastSpellHandleVFX;

	// Input ----------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_Interact;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_Sprint;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_LookMouse;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_LookStick;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_PrimaryAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_SecondaryAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_Dash;
	// Input ----------------------------------------
	
	FTraceHandle TraceHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FindCrosshairTarget();

	void CrosshairTraceComplete(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum);
	
	void Move(const FInputActionInstance& Instance);

	void LookMouse(const FInputActionValue& InputValue);

	void LookStick(const FInputActionValue& InputValue);

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

	UFUNCTION(BlueprintCallable)
	USAttributeComponent* GetAttributeComponent() const;

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100.f);

private:

	bool bHasPawnTarget;

};
