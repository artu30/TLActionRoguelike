#include "SCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SInteractionComponent.h"
#include "SAttackComponent.h"
#include "SAttributeComponent.h"
#include "Kismet/GameplayStatics.h"

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

	AttackComp = CreateDefaultSubobject<USAttackComponent>(TEXT("AttackComp"));

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnCharacterHealthChanged);
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.f;
	ControlRot.Roll = 0.f;
	
	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.f;
	ControlRot.Roll = 0.f;

	FVector RightVector = UKismetMathLibrary::GetRightVector(ControlRot);
	
	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
	if (!AttackComp)
	{
		return;
	}

	PlayAnimMontage(AttackComp->GetMagicProjectileAttackAnim());
	AttackComp->PrimaryAttack(GetWorld());
}

void ASCharacter::BlackholeAttack()
{
	if (!AttackComp)
	{
		return;
	}

	PlayAnimMontage(AttackComp->GetBlackholeProjectileAttackAnim());
	AttackComp->BlackholeAttack(GetWorld());
}

void ASCharacter::TeleportAttack()
{
	if (!AttackComp)
	{
		return;
	}

	PlayAnimMontage(AttackComp->GetTeleportProjectileAttackAnim());
	AttackComp->TeleportAttack(GetWorld());
}

void ASCharacter::PrimaryInteract()
{
	if (!InteractionComp)
	{
		return;
	}
	
	InteractionComp->PrimaryInteract();
}

void ASCharacter::OnCharacterHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (NewHealth <= 0.f && Delta < 0.f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
	else if (Delta < 0.f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -- Rotation Visualization -- //
	const float DrawScale = 100.0f;
	const float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart += GetActorRightVector() * 100.0f;
	// Set line end in direction of the actor's forward
	const FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	const FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("PrimaryAttack"), IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction(TEXT("BlackholeAttack"), IE_Pressed, this, &ASCharacter::BlackholeAttack);
	PlayerInputComponent->BindAction(TEXT("TeleportAttack"), IE_Pressed, this, &ASCharacter::TeleportAttack);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("PrimaryInteract"), IE_Pressed, this, &ASCharacter::PrimaryInteract);
}

FVector ASCharacter::GetAttackStartLocation() const
{
	return GetMesh()->GetSocketLocation(TEXT("Muzzle_01"));
}

FVector ASCharacter::GetAttackEndLocation() const
{
	FVector CameraLocation = CameraComp->GetComponentLocation();
	FVector EndCameraRayLocation = CameraLocation + (GetControlRotation().Vector() * 5000.f);

	FCollisionShape Shape;
	Shape.SetSphere(20.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	FHitResult Hit;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FVector ProjectileEndPosition = EndCameraRayLocation;
	if (GetWorld()->SweepSingleByObjectType(Hit, CameraLocation, EndCameraRayLocation, FQuat::Identity, ObjectQueryParams, Shape, Params))
	{
		ProjectileEndPosition = Hit.ImpactPoint;
	}

	return ProjectileEndPosition;
}

FRotator ASCharacter::GetAttackStartRotation() const
{
	return UKismetMathLibrary::FindLookAtRotation(GetAttackStartLocation(), GetAttackEndLocation());
}

USAttributeComponent* ASCharacter::GetAttributeComponent() const
{
	return AttributeComp;
}

void ASCharacter::SpawnCastSpellHandVFX()
{
	if (!CastSpellHandleVFX)
	{
		return;
	}

	FVector VFXLocation = GetAttackStartLocation();
	FRotator VFXRotation = GetMesh()->GetSocketRotation(TEXT("Muzzle_01"));
	
	UGameplayStatics::SpawnEmitterAttached(CastSpellHandleVFX, GetMesh(), "Muzzle_01", VFXLocation, VFXRotation, FVector(1), EAttachLocation::KeepWorldPosition);
}