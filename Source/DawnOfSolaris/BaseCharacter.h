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
	ABaseCharacter();

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

public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void updateMovement();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Current input component

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

	//FMovementData currentMovementData{ FMovementData(600.f, 540.f) };
	FMovementData currentMovementData{ FMovementData(maxWalkSpeed, maxRotationRate) };
	FMovementData defaultMovementData{ FMovementData(maxWalkSpeed, maxRotationRate) };
	FMovementData combatMovementData{ FMovementData(0.f, 50) };
	FMovementData hitstunMovementData{ FMovementData(0.f, 0.f) };

	void setMovementData(FMovementData inMovementData);

	UPROPERTY(BlueprintReadOnly)
	bool bIsDeafeated{ false };

	// TODO: Make this into an enum state(?)
	bool bAttackActionActive{ false }; // Active in attack frames
	bool bSelfHitstunActive{ false }; // Active in hitstun frames
	bool bStandbyActive{ true }; // Active when none of the above is active
	bool bDefaultAttackStarted{ false };
	
	UPROPERTY(BlueprintReadOnly)
	FAttackData currentAttackDataToSend;

	UPROPERTY(BlueprintReadOnly)
	FAttackData currentReceivedAttackData;


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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	ECombatAlignment getAlignment();
	virtual ECombatAlignment getAlignment_Implementation() override;

	void defaultAttackStartFromInput();
	void defaultAttackStart(int attackIndex = 0);
	void defaultAttackEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void startHitstun();
	virtual void startHitstun_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void endHitstun();
	virtual void endHitstun_Implementation() override;

	void cancelAttackActions(); // TODO: Complete this function

	void startIsDefeatedProcedure();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDefaultAttackData> defaultAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHitstunData> hitstunAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int currentPlayerID{ -1 };

	UAnimMontage* currentMontage;

	EActionType currentActionType;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite) // TODO: Complete this
	//TArray<FChargeAttackData> defaultComboTwoAttacks;

	//Move code below to player character

};
