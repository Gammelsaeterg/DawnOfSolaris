// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterInterface.h"

#include "BaseProjectile.generated.h"

UCLASS()
class DAWNOFSOLARIS_API ABaseProjectile : public AActor, public ICharacterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ProjectileMesh;

	// Projectile movement component
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float damageAmount{ 7.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float hitstunValue{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECombatAlignment CurrentProjectileCombatAlignment;

	class AActor* CurrentOwner;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class UParticleSystem* hitParticleEffectToSpawn{ nullptr };

	void setOwnerInfo(AActor* inOwner);

	UFUNCTION()
	void OnOverlapBeginProjectileHitbox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
										bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent) // TODO: Delete when this function no longer is needed
	void debugSpawnFireProjectileFX();

	UFUNCTION(BlueprintImplementableEvent) // TODO: Delete when this function no longer is needed
	void debugSpawnHitFX(FVector location);
};
