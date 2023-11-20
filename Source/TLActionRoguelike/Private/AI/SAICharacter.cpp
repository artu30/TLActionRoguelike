#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SDamagePopupUserWidget.h"
#include "SWorldUserWidget.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

	ActionComp = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComp"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnAICharacterHealthChanged);
}

AActor* ASAICharacter::GetTargetActor() const
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return nullptr;
	}

	return Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TargetActorKeyName));
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (GetTargetActor() == Pawn)
	{
		return;
	}
	
	SetTargetActor(Pawn);

	USWorldUserWidget* SpottedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
	if (SpottedWidget)
	{
		SpottedWidget->AttachedActor = this;

		// Index of 10 (or anything higher than default of 0) places this on top of any other widget.
		// May end up behind the minion health bar otherwise
		SpottedWidget->AddToViewport(10);
	}
}

void ASAICharacter::OnAICharacterHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta >= 0.f)
	{
		return;
	}
	
	if (InstigatorActor != this)
	{
		SetTargetActor(InstigatorActor);
	}

	if (ActiveHealthBar == nullptr && ensure(HealthBarWidgetClass))
	{
		ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
		if (ActiveHealthBar)
		{
			ActiveHealthBar->AttachedActor = this;
			ActiveHealthBar	->AddToViewport();
		}
	}

	if (!DamagePopUp && ensure(DamagePopUpWidgetClass))
	{
		DamagePopUp = CreateWidget<USDamagePopupUserWidget>(GetWorld(), DamagePopUpWidgetClass);
	}

	if (DamagePopUp)
	{
		if (!DamagePopUp->AttachedActor)
		{
			DamagePopUp->AttachedActor = this;
		}

		DamagePopUp->TextToShow = FMath::Abs(Delta);
		
		if (!DamagePopUp->IsInViewport())
		{
			DamagePopUp->AddToViewport();
		}
	}
	
	if (GetMesh())
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
	}
	
	if (NewHealth > 0.f)
	{
		return;
	}

	// Stop BT
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->GetBrainComponent()->StopLogic("Killed");
	}

	// Ragdoll
	if (GetMesh())
	{
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName("Ragdoll");
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	
	// Set lifespan
	SetLifeSpan(10.f);
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	BlackboardComp->SetValueAsObject("TargetActor", NewTarget);
}