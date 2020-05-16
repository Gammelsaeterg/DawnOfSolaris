// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacterMovementComponent.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DAWNOFSOLARIS_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:

	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// Custom movement setup
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE class UPlayerCharacterMovementComponent* GetPlayerCharacterMovementComponent() const { return PlayerCharacterMovementComponent; }
	UPlayerCharacterMovementComponent* PlayerCharacterMovementComponent;
	virtual void PostInitializeComponents() override;

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

	void BrowseUpPressed();
	void BrowseDownPressed();


	EActionType Action0Input{ EActionType::Interact }; EActionType InteractionMainActionType{ EActionType::DodgeRoll };
	EActionType Action1Input{ EActionType::DefaultComboOne };
	EActionType Action2Input{ EActionType::DefaultComboTwo };
	EActionType Action3Input{ EActionType::NONE };

	//EActionType BrowseUp // TODO(?): Complete these inputs in the same way as the action inputs
	//EActionType BrowseDown
	//void BrowseDownPressed();
	//void BrowseDownReleased();

	UPROPERTY(BlueprintReadOnly) // UPROPERTY is for debugging purposes // TODO: Delete later, remember to delete blueprints using this variable
	bool bChargeAttackInputHeld{ false };

	void actionPressed(EActionType inActionType);
	void actionReleased(EActionType inActionType);
	void actionReleased();

	void grabAttackPressed(); //TODO: Complete function
	void grabAttackReleased(); //TODO: Complete function

	void sprintPressed();
	void sprintReleased();

	void startDodgeRoll();

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
	float sprintStaminaCost{ 5.f };

	float maxSprintRotationRate{ 540.f };
	FMovementData sprintMovementData{ FMovementData(maxSprintSpeed, maxSprintRotationRate) };

	float currentChargeAttackStaminaConsumptionRate{ 0.f };

	UPROPERTY(BlueprintReadOnly)
	float tickWindUpChargeAmount{ 0.f };
	float currentChargeAmount{ 0.f };
	float currentRootAnimationMultiplier{ 1.f };

	bool bDodgingActive{ false }; // Active in dodge frames
	bool bSprintingActive{ false }; // Active when sprint button is held	
	bool bChargeAttackStarted{ false }; // Active when player is charghing attack
	bool bMinimumChargeReached{ false }; // Active after minimum charge notify is reached
	bool bCanSprintAttack{ false }; // Active while succesfully sprinting

	UPROPERTY(BlueprintReadOnly)
	bool bAttackHitboxActive{ false };

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> defaultComboOneAttacks;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> defaultComboTwoAttacks;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSprintAttackData sprintAttackOne;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSprintAttackData sprintAttackTwo;

	UPROPERTY(BlueprintReadOnly) // TODO: This is for debugging purposes, delete when no longer needed
	int currentMovesetIndex{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMovesetData> combatMovesets;

	TArray<FMovesetData> unlockedCombatMovesets;

	FMovesetData* currentMovesetData;

	FChargeAttackData currentChargeAttackDataToSend;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	FMovesetData getCurrentMovesetFromPlayer();
	virtual FMovesetData getCurrentMovesetFromPlayer_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	FMovesetData getNextMovesetFromPlayer();
	virtual FMovesetData getNextMovesetFromPlayer_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	FMovesetData getPreviousMovesetFromPlayer();
	virtual FMovesetData getPreviousMovesetFromPlayer_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	bool movesetsUnlocked();
	virtual bool movesetsUnlocked_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDodgeRollData currentDodgeRollData;

	//Hitbox collisions
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* LeftHandHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* RightHandHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* LeftFootHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* RightFootHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* LeftKneeHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* RightKneeHitbox;

	UPROPERTY(BlueprintReadOnly)
	EAttackHitboxType currentAttackHitboxType;

	//Tick functions
	void standbyCheckTick(); // Tick function to check if player is in standby
	void sprintTick(float DeltaTime); // Tick function to check if player can sprint while sprinting is active
	void regenStaminaTick(float DeltaTime);
	void regenHealthTick(float DeltaTime);
	void windUpChargeAmountTick(float deltaTime);
	void interactableTick(float deltaTime);
	

	bool canSprint();
	bool canRegenerateStamina();
	bool canAttack();
	bool canDodge();

	//Attack functions
	void incrementAttackCombo(EActionType inActionType);
	void resetAttackCombos();
	TArray<FChargeAttackData> getCurrentComboAttacks(EActionType inActionType, int inComboIndex = 2);

	void setMoveset(FMovesetData* inMovesetData);

	FMovesetData* findNextMoveset(bool setNewMoveset);
	FMovesetData* findPreviousMoveset(bool setNewMoveset);

	UFUNCTION(BlueprintCallable)
	FMovesetData getNextMoveset();

	UFUNCTION(BlueprintCallable)
	FMovesetData getPreviousMoveset();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void unlockMoveset(int movesetIndex);
	virtual void unlockMoveset_Implementation(int movesetIndex);

	void windUpChargeAttack(FChargeAttackData& inAttack);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void windUpStart();
	virtual void windUpStart_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void releaseStart();
	virtual void releaseStart_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void releaseEnd();
	virtual void releaseEnd_Implementation() override;

	void attackEnd();
	virtual void attackEnd_Implementation() override;

	void canCancelAction();
	virtual void canCancelAction_Implementation();

	void sprintAttack(EActionType inActionType);

	FAttackData calculateChargeAttackValues(FChargeAttackData inChargeAttackData);

	UFUNCTION()
	void OnOverlapBeginAttackHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								 bool bFromSweep, const FHitResult& SweepResult);

	FAttackData getCurrentAttackData(); // For player character projectiles

	void takeDamage(FAttackData inAttackData);
	virtual void takeDamage_Implementation(FAttackData inAttackData) override;

	virtual void hitstunReset() override;
	virtual void runHitstunProcedure(float inHitstunStrengthReceived, FVector hitDirection) override;

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getWindUpChargeAmount();
	virtual float getWindUpChargeAmount_Implementation() override;

//	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void activateAttackHitbox();
	virtual void activateAttackHitbox_Implementation() override;

//	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void deactivateAttackHitbox();
	virtual void deactivateAttackHitbox_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void startDodge();
	virtual void startDodge_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void endDodge();
	virtual void endDodge_Implementation();

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

	//Interaction
	void interact();

	class AInteractableObject* currentInteractableObject;
	
	UPROPERTY(BlueprintReadOnly) // TODO: This is for debugging purposes, delete when no longer needed
	bool bInteractableObjectInRange{ false };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void setInteractableObjectInRange(class AInteractableObject* inObject);
	virtual void setInteractableObjectInRange_Implementation(class AInteractableObject* inObject) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool getInteractableObjectInRange();
	virtual bool getInteractableObjectInRange_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
	void eventMovesetChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void eventCrystalAdded();

	UFUNCTION(BlueprintImplementableEvent)
	void eventStartChangeMoveset(bool nextMoveset);

	bool bChangeNextMoveset{ false };
	void startMovesetChange(bool nextMoveset);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void endMovesetChange();
	virtual void endMovesetChange_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* movesetChangeMontage{ nullptr };
};
