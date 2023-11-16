#include "SExplosiveBarrel.h"

#include "SAttributeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveBarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveBarrelMesh"));
	ExplosiveBarrelMesh->SetSimulatePhysics(true);
	ExplosiveBarrelMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	RootComponent = ExplosiveBarrelMesh;
	
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->Radius = 600.f;
	RadialForceComp->ImpulseStrength = 2500.f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
	RadialForceComp->SetupAttachment(ExplosiveBarrelMesh);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	ExplosiveBarrelMesh->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnHitBarrel);
}

void ASExplosiveBarrel::OnHitBarrel(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode(OtherActor);

	if (!OtherActor)
	{
		return;
	}

	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	if (!AttributeComp)
	{
		return;
	}

	AttributeComp->ApplyHealthChange(this, HitDamage);
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASExplosiveBarrel::Explode(AActor* OtherActor)
{
	RadialForceComp->FireImpulse();
	
	UE_LOG(LogTemp, Log, TEXT("Explosive Barrel - OtherActor: %s, at game time: %f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);
}

