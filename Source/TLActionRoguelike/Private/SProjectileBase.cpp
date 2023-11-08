#include "SProjectileBase.h"

#include "SAttributeComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ASProjectileBase::ASProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = SphereComp;

	EffectComp  = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->InitialSpeed = 1000.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(SphereComp);
}

// Called when the game starts or when spawned
void ASProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	AActor* ActorInstigator = Cast<AActor>(GetInstigator());
	SphereComp->IgnoreActorWhenMoving(ActorInstigator, true);
	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
	
	FTimerDelegate DetonationDelegate = FTimerDelegate::CreateUObject(this, &ASProjectileBase::OnDetonateProjectile);
	GetWorldTimerManager().SetTimer(TimerHandleDetonation, DetonationDelegate, TimeToDetonation, false);
}

void ASProjectileBase::OnDetonateProjectile()
{
	if (IsValid(CameraShake))
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShake, GetActorLocation(), CameraShakeMinRadius, CameraShakeMaxRadius);
	}
	
	SpawnImpactSound();
	
	SpawnImpactVFX();

	OnDestroyProjectile();
}

void ASProjectileBase::SpawnImpactVFX()
{
	if (!ImpactVFX)
	{
		return;
	}
	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, GetActorLocation(), GetActorRotation(), false);
}

void ASProjectileBase::SpawnImpactSound()
{
	if (!ImpactSound)
	{
		return;
	}
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation(), GetActorRotation());
}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* ActorInstigator = Cast<AActor>(GetInstigator());
	if (OtherActor != ActorInstigator)
	{
		OnDetonateProjectile();
		
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (!AttributeComp || !OtherActor)
		{
			return;
		}

		AttributeComp->ApplyHealthChange(HitDamage);
	}
}
