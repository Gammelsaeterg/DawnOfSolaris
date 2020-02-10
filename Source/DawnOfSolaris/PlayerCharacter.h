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

	void comboAttackPressed(EActionType inActionType);
	void comboAttackReleased(EActionType inActionType);
	void updateComboMaxIndexes();
	void updateCurrentIndexes();

	void Action0Pressed();
	void Action1Pressed();
	void Action2Pressed();
	void Action3Pressed();

	void Action0Released();
	void Action1Released();
	void Action2Released();
	void Action3Released();

	EActionType Action0Input{ EActionType::Interact };
	EActionType Action1Input{ EActionType::DefaultComboOne };
	EActionType Action2Input{ EActionType::DefaultComboTwo };
	EActionType Action3Input{ EActionType::GrabAttack };

	UPROPERTY(BlueprintReadOnly) // UPROPERTY is for debugging purposes // TODO: Delete later, remember to delete blueprints using this variable
	bool bChargeAttackInputHeld{ false };

	void actionPressed(EActionType inActionType);
	void actionReleased(EActionType inActionType);
	void actionReleased();

	void incrementAttackCombo(EActionType inActionType);
	TArray<FChargeAttackData> getCurrentMoveset(EActionType inActionType, int inMovesetIndex = 2);

	void grabAttackPressed(); //TODO: Complete function
	void grabAttackReleased(); //TODO: Complete function

	void sprintPressed();
	void sprintReleased();

	// AI input
	UFUNCTION(BlueprintCallable)
	void attackAI(EActionType inAttackCombo, float chargeAmount); //TODO make attack index into enum
	FTimerHandle attackTimerAI;

	//Variables
	UPROPERTY(BlueprintReadOnly)
	TArray<int> maxComboIndexes;
	UPROPERTY(BlueprintReadOnly)
	TArray<int> currentComboIndexes;


	UPROPERTY(BlueprintReadOnly)
	int defaultComboOneMaxIndex{ 1 };
	UPROPERTY(BlueprintReadOnly)
	int defaultComboTwoMaxIndex{ 1 };

	UPROPERTY(BlueprintReadOnly)
	int defaultComboOneCurrentIndex{ 0 };
	UPROPERTY(BlueprintReadOnly)
	int defaultComboTwoCurrentIndex{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxStaminaPoints{ 100 };
	float currentStaminaPoints{ 100 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxSprintSpeed{ 1100 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float baseStaminaRegen{ 25.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float sprintStaminaCost{ 45.f };

	float currentChargeAmount{ 0.f };

	bool bDodgingActive{ false }; // Active in dodge frames
	bool bSprintingActive{ false }; // Active when sprint button is held	
	bool bChargeAttackStarted{ false }; // Active when player is charghing attack
	bool bMinimumChargeReached{ false }; // Active after minimum charge notify is reached

	UPROPERTY(BlueprintReadOnly)
	bool bAttackHitboxActive{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> defaultComboOneAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> defaultComboTwoAttacks;

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
	void releaseStart();
	virtual void releaseStart_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void releaseEnd();
	virtual void releaseEnd_Implementation() override;

	FAttackData calculateChargeAttackValues(FChargeAttackData inChargeAttackData);

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

	void startHitstun();
	virtual void startHitstun_Implementation() override;

	void endHitstun();
	virtual void endHitstun_Implementation() override;

	//Other functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getStaminaPoints();
	virtual float getStaminaPoints_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getMaxStaminaPoints();
	virtual float getMaxStaminaPoints_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setStaminaPoints(float newStaminaPoints);
	virtual void setStaminaPoints_Implementation(float newStaminaPoints) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void activateAttackHitbox();
	virtual void activateAttackHitbox_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void deactivateAttackHitbox();
	virtual void deactivateAttackHitbox_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setChargeAmount(float newChargeAmount); // TODO: Charge determined based on montage position, should be based on how far in it is compared to the charge/windUp animation
	virtual void setChargeAmount_Implementation(float newChargeAmount) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsWindingUpChargeAttack();
	virtual bool getIsWindingUpChargeAttack_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void minimumChargeAmountReached();
	virtual void minimumChargeAmountReached_Implementation();

	//Hitbox handling
	void enableHitbox(EAttackHitboxType inHitbox, bool enabled);
	void disableAllHitboxes();

	//Storing current hit actors to avoid multiple hits in one attack
	TArray<AActor*> hitActors;
	bool isActorAlreadyHit(AActor* inActor);
	void clearHitActors();
};
