// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlapBeginProjectileHitbox);
	RootComponent = ProjectileMesh;
	ProjectileMesh->SetGenerateOverlapEvents(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = 1600.0f;
	ProjectileMovementComponent->MaxSpeed = 1600.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	//ProjectileMovementComponent->Velocity = GetActorRotation().Vector() * ProjectileMovementComponent->InitialSpeed;
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseProjectile::setOwnerInfo(AActor* inOwner)
{
	CurrentOwner = inOwner;
	CurrentProjectileCombatAlignment = Execute_getAlignment(CurrentOwner);
}

void ABaseProjectile::OnOverlapBeginProjectileHitbox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
													 UPrimitiveComponent * OtherComp, int32 OtherBodyIndex,
													 bool bFromSweep, const FHitResult & SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Overlapped actor: %s"), *OtherActor->GetName()); //// Debug texts, very nice and valuable 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (CurrentOwner != OtherActor)) // Default nullptr and self check)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Overlapped actor: %s"), *OtherActor->GetName()); //// Debug texts, very nice and valuable 
		ICharacterInterface* characterInterface = Cast<ICharacterInterface>(OtherActor);
		if (characterInterface)
		{
			if (canDamageInteract(CurrentProjectileCombatAlignment, characterInterface->Execute_getAlignment(OtherActor)))
			{
				FVector hitDirection;
				hitDirection = ProjectileMesh->GetForwardVector();

				FAttackData currentAttackDataToSend = FAttackData(damageAmount,
															      hitDirection, SweepResult.Location,
																  this, hitstunValue);

				//SetLifeSpan(1.f); // TODO: For debug purposes, remove when no longer needed
				characterInterface->Execute_takeDamage(OtherActor, currentAttackDataToSend);
			}
		}

		//if (hitParticleEffectToSpawn)
		//{
		//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hitParticleEffectToSpawn, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, EPSCPoolMethod::None, true);
		//}
		debugSpawnHitFX(GetActorLocation());
		
		// When projectile hits something
		// TODO: Place FX hit effects here
		Destroy();
	}
}