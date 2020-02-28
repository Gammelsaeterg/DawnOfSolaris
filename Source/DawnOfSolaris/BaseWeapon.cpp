// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionMesh"));
	CollisionMesh->SetupAttachment(RootComponent);
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnOverlapBeginWeaponHitbox);
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
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
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) // Default nullptr and self check // (OtherComp->IsA(USkeletalMesh::StaticClass()))
	{
		ICharacterInterface* characterInterface = Cast<ICharacterInterface>(OtherActor);
		if (characterInterface && !(isActorAlreadyHit(OtherActor)))
		{
			FVector hitDirection;
			hitDirection = OverlappedComp->GetPhysicsLinearVelocity().GetSafeNormal(0.000001f);
			if (hitDirection.Size() < 1.f) // If getting physics velocity fails
			{
				//hitDirection = GetMesh()->GetRightVector().GetSafeNormal(0.000001f);
				hitDirection = CollisionMesh->GetForwardVector().GetSafeNormal(0.000001f); // TODO(?): Unsure of this is correct
			}

			FAttackData currentAttackDataToSend = FAttackData(CurrentMeleeWeaponAttackData.damageValue,
															  hitDirection, SweepResult.Location,
															  this, CurrentMeleeWeaponAttackData.hitstunValue);

			characterInterface->Execute_takeDamage(OtherActor, currentAttackDataToSend);
		}
	}
}

void ABaseWeapon::sendAttackDataToWeapon_Implementation(FDefaultAttackData inAttackData, ECombatAlignment inAlignment)
{
	CurrentMeleeWeaponAttackData = inAttackData;
	CurrentWeaponCombatAlignment = inAlignment;
}

void ABaseWeapon::activateAttackHitbox_Implementation()
{
	CollisionMesh->SetGenerateOverlapEvents(true);
	CollisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ABaseWeapon::deactivateAttackHitbox_Implementation()
{
	CollisionMesh->SetGenerateOverlapEvents(false);
	CollisionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	clearHitActors();
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

