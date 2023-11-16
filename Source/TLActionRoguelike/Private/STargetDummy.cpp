#include "STargetDummy.h"

#include "SAttributeComponent.h"
#include "SDamagePopupUserWidget.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASTargetDummy::ASTargetDummy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));
}

// Called when the game starts or when spawned
void ASTargetDummy::BeginPlay()
{
	Super::BeginPlay();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASTargetDummy::OnTargetDummyHealthChanged);
}

void ASTargetDummy::OnTargetDummyHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta >= 0.f)
	{
		return;
	}

	if (DamagePopUp == nullptr)
	{
		DamagePopUp = CreateWidget<USDamagePopupUserWidget>(GetWorld(), DamagePopUpWidgetClass);
		if (DamagePopUp)
		{
			DamagePopUp->AttachedActor = this;
			DamagePopUp->TextToShow = FMath::Abs(Delta);
			DamagePopUp->AddToViewport();
		}
	}
	
	MeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
}