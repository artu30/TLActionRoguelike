#include "SCharacter.h"

#include "SActionComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SPlayerController.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

	ActionComp = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComp"));
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnCharacterHealthChanged);
}

void ASCharacter::FindCrosshairTarget()
{
	// Ignore if not using GamePad
	ASPlayerController* PlayerController = Cast<ASPlayerController>(GetController());

	// Only use aim assist when currently controlled and using gamepad
	// Note: you *may* always want to line trace if using this result for other things like coloring crosshair or re-using this hit data for aim adjusting during projectile attacks
	if (PlayerController == nullptr || !PlayerController->IsUsingGamepad())
	{
		bHasPawnTarget = false;
		return;
	}

	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const float AimAssistDistance = 5000.f;
	const FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * AimAssistDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionShape Shape;
	Shape.SetSphere(50.f);

	// Called next frame when the trace has completed
	FTraceDelegate Delegate = FTraceDelegate::CreateUObject(this, &ASCharacter::CrosshairTraceComplete);
	
	TraceHandle = GetWorld()->AsyncSweepByChannel(EAsyncTraceType::Single, EyeLocation, TraceEnd, FQuat::Identity, ECC_Pawn, Shape, Params, FCollisionResponseParams::DefaultResponseParam, &Delegate);
}


void ASCharacter::CrosshairTraceComplete(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum)
{
	// at most expect one hit
	if (InTraceDatum.OutHits.IsValidIndex(0))
	{
		FHitResult Hit = InTraceDatum.OutHits[0];
		// Figure out if dealing with a Pawn, may want aim assist on other 'things', which requires a different check
		bHasPawnTarget = Hit.IsValidBlockingHit() && Hit.GetActor()->IsA(APawn::StaticClass());
	}
}

void ASCharacter::Move(const FInputActionInstance& Instance)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.f;
	ControlRot.Roll = 0.f;

	// Get value from input (combined value from WASD keys or single Gamepad stick) and convert to Vector (x,y)
	const FVector2d AxisValue = Instance.GetValue().Get<FVector2d>();

	// Move forward/back
	AddMovementInput(ControlRot.Vector(), AxisValue.Y);

	// Move right/left strafe
	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, AxisValue.X);
}

void ASCharacter::LookMouse(const FInputActionValue& InputValue)
{
	const FVector2d Value = InputValue.Get<FVector2d>();

	AddControllerYawInput(Value.X);
	AddControllerPitchInput(Value.Y);
}

void ASCharacter::LookStick(const FInputActionValue& InputValue)
{
	FVector2D Value = InputValue.Get<FVector2D>();

	// Track negative as we'll lose this during the conversion
	bool XNegative = Value.X < 0.f;
	bool YNegative = Value.Y < 0.f;

	// Can further modify with 'sensitivity' settings
	static const float LookYawRate = 100.0f;
	static const float LookPitchRate = 50.0f;

	// non-linear to make aiming a little easier
	Value = Value * Value;

	if (XNegative)
	{
		Value.X *= -1.f;
	}
	if (YNegative)
	{
		Value.Y *= -1.f;
	}

	// Aim assist
	// todo: may need to ease this out and/or change strength based on distance to target
	float RateMultiplier = 1.0f;
	if (bHasPawnTarget)
	{
		RateMultiplier = 0.5f;
	}

	AddControllerYawInput(Value.X * (LookYawRate * RateMultiplier) * GetWorld()->GetDeltaSeconds());
	AddControllerPitchInput(Value.Y * (LookPitchRate * RateMultiplier) * GetWorld()->GetDeltaSeconds());
}

void ASCharacter::PrimaryAttack()
{
	if (!ActionComp)
	{
		return;
	}
	
	ActionComp->StartActionByName(this, "PrimaryAttack");
}

void ASCharacter::BlackholeAttack()
{
	if (!ActionComp)
	{
		return;
	}
	
	ActionComp->StartActionByName(this, "BlackholeAttack");
}

void ASCharacter::TeleportAttack()
{
	if (!ActionComp)
	{
		return;
	}
	
	ActionComp->StartActionByName(this, "TeleportAttack");
}

void ASCharacter::PrimaryInteract()
{
	if (!InteractionComp)
	{
		return;
	}
	
	InteractionComp->PrimaryInteract();
}

void ASCharacter::SprintStart()
{
	if (!ActionComp)
	{
		return;
	}
	
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::SprintStop()
{
	if (!ActionComp)
	{
		return;
	}
	
	ActionComp->StopActionByName(this, "Sprint");
}

void ASCharacter::OnCharacterHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (NewHealth <= 0.f && Delta < 0.f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);

		if (AttributeComp)
		{
			AttributeComp->RemoveRage(this);
		}
		
		SetLifeSpan(5.f);
	}
	else if (Delta < 0.f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}

FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FindCrosshairTarget();
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const APlayerController* PlayerController = GetController<APlayerController>();
	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	// Add mappings for our game, more complex games may have multiple contexts that are added/removed at runtime
	Subsystem->AddMappingContext(DefaultInputMapping, 0);

	// New Enhanced Input system
	UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// General
	InputComp->BindAction(Input_Move, ETriggerEvent::Triggered, this, &ASCharacter::Move);
	InputComp->BindAction(Input_Jump, ETriggerEvent::Triggered, this, &ASCharacter::Jump);
	InputComp->BindAction(Input_Interact, ETriggerEvent::Triggered, this, &ASCharacter::PrimaryInteract);

	// Sprint while key is held
	InputComp->BindAction(Input_Sprint, ETriggerEvent::Started, this, &ASCharacter::SprintStart);
	InputComp->BindAction(Input_Sprint, ETriggerEvent::Completed, this, &ASCharacter::SprintStop);

	// MKB
	InputComp->BindAction(Input_LookMouse, ETriggerEvent::Triggered, this, &ASCharacter::LookMouse);
	// Gamepad
	InputComp->BindAction(Input_LookStick, ETriggerEvent::Triggered, this, &ASCharacter::LookStick);

	// Abilities
	InputComp->BindAction(Input_PrimaryAttack, ETriggerEvent::Triggered, this, &ASCharacter::PrimaryAttack);
	InputComp->BindAction(Input_SecondaryAttack, ETriggerEvent::Triggered, this, &ASCharacter::BlackholeAttack);
	InputComp->BindAction(Input_Dash, ETriggerEvent::Triggered, this, &ASCharacter::TeleportAttack);
	
	/* OLD INPUT
	InputComp->BindAxis(TEXT("MoveForward"), this, &ASCharacter::MoveForward);
	InputComp->BindAxis(TEXT("MoveRight"), this, &ASCharacter::MoveRight);

	InputComp->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	InputComp->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	InputComp->BindAction(TEXT("PrimaryAttack"), IE_Pressed, this, &ASCharacter::PrimaryAttack);
	InputComp->BindAction(TEXT("BlackholeAttack"), IE_Pressed, this, &ASCharacter::BlackholeAttack);
	InputComp->BindAction(TEXT("TeleportAttack"), IE_Pressed, this, &ASCharacter::TeleportAttack);

	InputComp->BindAction(TEXT("Jump"), IE_Pressed, this, &ASCharacter::Jump);

	InputComp->BindAction(TEXT("PrimaryInteract"), IE_Pressed, this, &ASCharacter::PrimaryInteract);

	InputComp->BindAction(TEXT("Sprint"), IE_Pressed, this, &ASCharacter::SprintStart);
	InputComp->BindAction(TEXT("Sprint"), IE_Released, this, &ASCharacter::SprintStop);
	*/
}

USAttributeComponent* ASCharacter::GetAttributeComponent() const
{
	return AttributeComp;
}

void ASCharacter::HealSelf(float Amount /* = 100 */)
{
	if (!AttributeComp)
	{
		return;
	}

	AttributeComp->ApplyHealthChange(this, Amount);
}