// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "BaseCharacter.h"
#include "BaseProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionMesh"));
	CollisionMesh->SetupAttachment(RootComponent);
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnOverlapBeginWeaponHitbox);
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	//CollisionMesh->SetRelativeLocation(FVector::ZeroVector); // TODO(?): This should not be necessary
	WeaponMesh->SetMobility(EComponentMobility::Movable); // TODO(?): This should not be necessary
	CollisionMesh->SetMobility(EComponentMobility::Movable); // TODO(?): This should not be necessary
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseWeapon::OnOverlapBeginWeaponHitbox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
											 UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, 
											 bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (CurrentCharacterOwner != OtherActor)) // Default nullptr and self check // (OtherComp->IsA(USkeletalMesh::StaticClass()))
	{
		ICharacterInterface* characterInterface = Cast<ICharacterInterface>(OtherActor);
		if (characterInterface && !(isActorAlreadyHit(OtherActor)))
		{
			if (canDamageInteract(CurrentWeaponCombatAlignment, characterInterface->Execute_getAlignment(OtherActor)))
			{
				hitActors.Add(OtherActor);
				//UE_LOG(LogTemp, Warning, TEXT("Overlapped self comp: %s"), *OverlappedComp->GetName()); //// Debug texts, very nice and valuable 
				FVector hitDirection;
				hitDirection = FVector(OverlappedComp->GetPhysicsLinearVelocity().X, OverlappedComp->GetPhysicsLinearVelocity().Y, 0.f).GetSafeNormal(0.000001f);
				if (hitDirection.Size() < 1.f) // If getting physics velocity fails
				{
					hitDirection = CurrentCharacterOwner->GetMesh()->GetRightVector().GetSafeNormal(0.000001f); // TODO(?): Red error line, fix
				}

				FAttackData currentAttackDataToSend = FAttackData(CurrentMeleeWeaponAttackData.damageValue,
													  hitDirection, SweepResult.Location,
													  this, CurrentMeleeWeaponAttackData.hitstunValue);

				characterInterface->Execute_takeDamage(OtherActor, currentAttackDataToSend);
				debugSpawnFireFX(WeaponMesh->GetComponentTransform());
			}
		}
	}
}

void ABaseWeapon::sendAttackDataToWeapon_Implementation(FDefaultAttackData inAttackData, ECombatAlignment inAlignment, ABaseCharacter* inOwner)
{
	CurrentMeleeWeaponAttackData = inAttackData;
	CurrentWeaponCombatAlignment = inAlignment;
	CurrentCharacterOwner = inOwner;
}

void ABaseWeapon::activateAttackHitbox_Implementation()
{
	UPrimitiveComponent* overlapCollisionToEnable = CollisionMesh; // TODO(?): Redundant?

	overlapCollisionToEnable->SetGenerateOverlapEvents(true);
	overlapCollisionToEnable->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ABaseWeapon::deactivateAttackHitbox_Implementation()
{
	UPrimitiveComponent* overlapCollisionToEnable = CollisionMesh; // TODO(?): Redundant?

	overlapCollisionToEnable->SetGenerateOverlapEvents(false);
	overlapCollisionToEnable->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	clearHitActors();
}

void ABaseWeapon::fireProjectile_Implementation()
{
	if (WeaponProjectile != nullptr)
	{
		FTransform spawnTransform = FTransform(CurrentCharacterOwner->GetMesh()->GetRightVector().ToOrientationRotator(), // Using parent actor's rotation instead of object rotation
											   WeaponMesh->GetSocketLocation("ProjectileSpawnLocation"), 
			                                   WeaponMesh->GetRelativeScale3D());
		auto ProjectileToSpawn = UGameplayStatics::BeginDeferredActorSpawnFromClass(this, WeaponProjectile, spawnTransform);

		Cast<ABaseProjectile>(ProjectileToSpawn)->setOwnerInfo(CurrentCharacterOwner);

		UGameplayStatics::FinishSpawningActor(ProjectileToSpawn, ProjectileToSpawn->GetTransform());

		debugSpawnFireFX(WeaponMesh->GetSocketTransform("ProjectileSpawnLocation"));
	}
}

void ABaseWeapon::detachWeapon_Implementation()
{
	Execute_deactivateAttackHitbox(this);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	WeaponMesh->DetachFromParent(true);
	//WeaponMesh->SetCollisionObjectType(ECollisionChannel::ECC_Destructible);
	//WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionProfileName("Ragdoll");
	WeaponMesh->SetSimulatePhysics(true);
}

bool ABaseWeapon::isActorAlreadyHit(AActor * inActor)
{
	// TODO: Simplify this
	bool bIsActorAlreadyHit{ false };
	for (AActor * hitActor : hitActors)
	{
		if (inActor == hitActor)
		{
			bIsActorAlreadyHit = true;
		}
	}

	if (bIsActorAlreadyHit)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ABaseWeapon::clearHitActors()
{
	hitActors.Empty();
}

