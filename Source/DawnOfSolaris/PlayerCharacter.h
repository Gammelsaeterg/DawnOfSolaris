// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DAWNOFSOLARIS_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:

	APlayerCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//Inputs
	void attackOnePressed();
	void attackOneReleased();

	void attackTwoPressed();
	void attackTwoReleased();

	void grabAttackPressed(); //TODO: Complete function
	void grabAttackReleased(); //TODO: Complete function

	void sprintPressed();
	void sprintReleased();

	// AI input
	UFUNCTION(BlueprintCallable)
	void attackAI(int attackIndex, float chargeAmount); //TODO make attack index into enum
	FTimerHandle attackTimerAI;

	//Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxStaminaPoints{ 100 };
	float currentStaminaPoints{ 100 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxSprintSpeed{ 1100 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float baseStaminaRegen{ 25.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float sprintStaminaCost{ 45.f };

	bool bDodgingActive{ false }; // Active in dodge frames
	bool bSprintingActive{ false }; // Active when sprint button is held	
	bool bChargeAttackStarted{ false };

	UPROPERTY(BlueprintReadOnly)
	bool bAttackHitboxActive{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> attackOneAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> attackTwoAttacks;

	//Hitbox collisions
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* LeftHandHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* RightHandHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* LeftFootHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* RightFootHitbox;

	UPROPERTY(BlueprintReadOnly)
	EAttackHitboxType currentAttackHitboxType;

	//Tick functions
	void standbyCheckTick(); // Tick function to check if player is in standby
	void sprintTick(float DeltaTime); // Tick function to check if player can sprint while sprinting is active
	void regenStaminaTick(float DeltaTime);

	bool canSprint();
	bool canRegenerateStamina();
	bool canAttack();

	//Attack functions
	void windUpChargeAttack(FChargeAttackData& inAttack);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void releaseAttack();
	virtual void releaseAttack_Implementation() override;

	//Hitbox overlap events
	UFUNCTION()
	void OnOverlapBeginLeftHandHitbox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
									  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
									  bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapBeginRightHandHitbox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									   bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapBeginLeftFootHitbox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									  bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapBeginRightFootHitbox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								       bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapBeginAttackHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								 bool bFromSweep, const FHitResult& SweepResult);

	void takeDamage(FAttackData inAttackData);
	virtual void takeDamage_Implementation(FAttackData inAttackData) override;

	//Other functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getStaminaPoints();
	virtual float getStaminaPoints_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setStaminaPoints(float newStaminaPoints);
	virtual void setStaminaPoints_Implementation(float newStaminaPoints) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void activateAttackHitbox();
	virtual void activateAttackHitbox_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void deactivateAttackHitbox();
	virtual void deactivateAttackHitbox_Implementation() override;

	//Hitbox handling
	void enableHitbox(EAttackHitboxType inHitbox, bool enabled);
	void disableAllHitboxes();

	//Storing current hit actors to avoid multiple hits in one attack
	TArray<AActor*> hitActors;
	bool isActorAlreadyHit(AActor* inActor);
	void clearHitActors();
};
