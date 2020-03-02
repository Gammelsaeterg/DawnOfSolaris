// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterInterface.h"
#include "Components/InputComponent.h"
#include "Containers/Array.h"
#include "DawnOfSolaris.h"

#include "BaseCharacter.generated.h"

class UAnimMontage;

UCLASS(config = Game)
class DAWNOFSOLARIS_API ABaseCharacter : public ACharacter, public ICharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Current input component

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
 * Called via input to turn at a given rate.
 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Camera boom positioning the camera behind the character */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Outer collision sphere, used for destructible handling **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* OuterCapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* Weapon{ nullptr };

public:	

	void movementSmoothingTick(float DeltaTime);
	void launchedTick(float DeltaTime);

	void updateMovement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	ECombatAlignment CombatAlignment = ECombatAlignment::Neutral;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxHealthPoints{ 100 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float currentHealthPoints{ 100 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxWalkSpeed{ 600.f }; // Also max regular movement speed

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxRotationRate{ 540.f }; //

	float defaultCapsuleHalfHeight{ 96.f };
	float launchedCapsuleHalfHeight{ 22.f };

	//FMovementData currentMovementData{ FMovementData(600.f, 540.f) };
	FMovementData currentMovementData{ FMovementData(maxWalkSpeed, maxRotationRate) };
	FMovementData defaultMovementData{ FMovementData(maxWalkSpeed, maxRotationRate) };
	FMovementData combatMovementData{ FMovementData(0.f, 50) };
	FMovementData hitstunMovementData{ FMovementData(0.f, 0.f) };

	//void setMovementData(FMovementData inMovementData);

	UPROPERTY(BlueprintReadOnly)
	bool bIsDeafeated{ false };

	// TODO: Make this into an enum state(?)
	UPROPERTY(BlueprintReadOnly) // UPROPERTY for debugging purposes // TODO: Delete later, remember to delete referenced blueprints
	bool bAttackActionActive{ false }; // Active in attack frames
	bool bSelfHitstunActive{ false }; // Active in hitstun frames
	bool bStandbyActive{ true }; // Active when none of the above is active
	bool bDefaultAttackStarted{ false };
	bool bCanCancelAction{ false }; // Active after can cancel action notify is reached

	UPROPERTY(BlueprintReadOnly)
	bool bIsLaunched{ false }; // Active after struck by an attack that launches (hitstun valiue larger than 0.7f)

	UPROPERTY(BlueprintReadOnly)
	bool bIsGrounded{ false }; // Active after a launch and character lies flat on ground

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsAttacking();
	virtual bool getIsAttacking_Implementation() override;
	
	UPROPERTY(BlueprintReadOnly)
	FAttackData currentAttackDataToSend;

	UPROPERTY(BlueprintReadOnly)
	FAttackData currentReceivedAttackData;

	UPROPERTY(BlueprintReadOnly)
	FDefaultAttackData currentDefaultAttackData;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsHitstunned();
	virtual bool getIsHitstunned_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsLaunched();
	virtual bool getIsLaunched_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsGrounded();
	virtual bool getIsGrounded_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getHealthPoints();
	virtual float getHealthPoints_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getMaxHealthPoints();
	virtual float getMaxHealthPoints_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setHealthPoints(float newHealthPoints);
	virtual void setHealthPoints_Implementation(float newHealthPoints) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void takeDamage(FAttackData inAttackData);
	virtual void takeDamage_Implementation(FAttackData inAttackData) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void attackStart();
	void attackStart_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void attackEnd();
	void attackEnd_Implementation() override;

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
	void canCancelAction();
	virtual void canCancelAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	ECombatAlignment getAlignment();
	virtual ECombatAlignment getAlignment_Implementation() override;

	void defaultAttackStartFromInputOne();
	void defaultAttackStartFromInputTwo();
	void defaultAttackStart(int attackIndex = 0);
	void defaultAttackEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void startHitstun();
	virtual void startHitstun_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void endHitstun();
	virtual void endHitstun_Implementation() override;

	// Hitstun calculation: hitstun < 0.1f: hitstunAnimationOnly, 0.1f - 0.3f: hitstunFlinch, 0.3f - 0.7f: hitstunFlinchWithKnockback, > 0.7f: hitstunLaunched
	void runHitstunProcedure(float inHitstunStrengthReceived, FVector hitDirection);

	void startLaunch();
	void endLaunch();

	void cancelAttackActions(); // TODO: Complete this function

	void startIsDefeatedProcedure();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDefaultAttackData> defaultAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitstunData hitstunAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int currentPlayerID{ -1 };

	UAnimMontage* currentMontage;

	UPROPERTY(BlueprintReadOnly) // UPROPERTY for debugging purposes // TODO: Delete later, remember to delete referenced blueprints
	EActionType currentActionType;

	bool bQueueAttacks{ true };
	UPROPERTY(BlueprintReadOnly) // UPROPERTY for debugging purposes // TODO: Delete later, remember to delete referenced blueprints
	bool bQueuedInputHeld{ false };
	UPROPERTY(BlueprintReadOnly) // UPROPERTY for debugging purposes // TODO: Delete later, remember to delete referenced blueprints
	EActionType lastQueuedActionHeld;

	UPROPERTY(BlueprintReadOnly) // UPROPERTY for debugging purposes // TODO: Delete later, remember to delete referenced blueprints
	TArray<EActionType> queuedActionTypes;

	UFUNCTION(BlueprintImplementableEvent) // TODO: Delete when this function no longer is needed
	void debugFunctionForBlueprint();

	UFUNCTION(BlueprintImplementableEvent) // TODO: Delete when this function no longer is needed
	void debugSpawnFX();

	UFUNCTION(BlueprintImplementableEvent) // TODO: Delete when this function no longer is needed
	void debugDespawnFX();

	UFUNCTION(BlueprintImplementableEvent) // TODO: Delete when this function no longer is needed
	void debugSpawnHitFX(FVector hitLocation);
};
