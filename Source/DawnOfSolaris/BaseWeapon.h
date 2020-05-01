// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DawnOfSolaris.h"
#include "CharacterInterface.h"

#include "BaseWeapon.generated.h"


UCLASS()
class DAWNOFSOLARIS_API ABaseWeapon : public AActor, public ICharacterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CollisionMesh;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class UStaticMeshComponent* ProjectileMesh;

	FDefaultAttackData CurrentMeleeWeaponAttackData;
	ECombatAlignment CurrentWeaponCombatAlignment;	
	class ABaseCharacter* CurrentCharacterOwner;
	TArray<AActor*> hitActors; //Storing current hit actors to avoid multiple hits in one attack

	UFUNCTION()
	void OnOverlapBeginWeaponHitbox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
									bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void sendAttackDataToWeapon(FDefaultAttackData inAttackData, ECombatAlignment inAlignment, ABaseCharacter* inOwner);
	virtual void sendAttackDataToWeapon_Implementation(FDefaultAttackData inAttackData, ECombatAlignment inAlignment, ABaseCharacter* inOwner) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void activateAttackHitbox();
	virtual void activateAttackHitbox_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void deactivateAttackHitbox();
	virtual void deactivateAttackHitbox_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void fireProjectile();
	virtual void fireProjectile_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void detachWeapon();
	virtual void detachWeapon_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void setWeaponVisibility(bool isVisible);
	virtual void setWeaponVisibility_Implementation(bool isVisible);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseProjectile> WeaponProjectile;

	bool isActorAlreadyHit(AActor* inActor);
	void clearHitActors();

	UFUNCTION(BlueprintImplementableEvent) // TODO: Delete when this function no longer is needed
	void debugSpawnFireFX(FTransform muzzleTransform);

	UFUNCTION(BlueprintImplementableEvent) // TODO: Delete when this function no longer is needed
	void debugSpawnHitFX(FVector location);
};
