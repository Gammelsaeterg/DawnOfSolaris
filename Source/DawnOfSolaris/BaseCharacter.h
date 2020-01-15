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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Current input component

	float maxHealthPoints{ 100 };
	float currentHealthPoints{ 100 };

	float maxStaminaPoints{ 100 };
	float currentStaminaPoints{ 100 };

	float maxWalkSpeed{ 600 };
	float maxSprintSpeed{ 1100 };

	// TODO: Make this into an enum state(?)
	bool bAttackActionActive{ false }; // Active in attack frames
	bool bSelfHitstunActive{ false }; // Active in hitstun frames
	bool bDodgingActive{ false }; // Active in dodge frames
	bool bSprintingActive{ false }; // Active when sprint button is held
	bool bStandbyActive{ true }; // Active when none of the above is active

	bool bChargeAttackStarted{ false };


	UPROPERTY(BlueprintReadOnly)
	int attackOneComboMaxIndex{ 1 };
	UPROPERTY(BlueprintReadOnly)
	int attackTwoComboMaxIndex{ 1 };

	UPROPERTY(BlueprintReadOnly)
	int attackOneComboCurrentIndex{ 0 };
	UPROPERTY(BlueprintReadOnly)
	int attackTwoComboCurrentIndex{ 0 };

	void incrementAttackCombo();


	float baseStaminaRegen{ 25.f };
	float sprintStaminaCost{ 45.f };


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getHealthPoints();
	virtual float getHealthPoints_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setHealthPoints(float newHealthPoints);
	virtual void setHealthPoints_Implementation(float newHealthPoints) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getStaminaPoints();
	virtual float getStaminaPoints_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setStaminaPoints(float newStaminaPoints);
	virtual void setStaminaPoints_Implementation(float newStaminaPoints) override;

	void sprintActivate();
	void sprintDeactivate();

	void attackOnePressed();
	void attackOneReleased();

	void attackTwoPressed();
	void attackTwoReleased();

	void cancelAttackActions(); // TODO: Complete this function

	bool canSprint();
	bool canRegenerateStamina();
	bool canAttack();

	void windUpChargeAttack(FChargeAttackData& inAttack);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void releaseAttack();
	virtual void releaseAttack_Implementation() override;

	void standbyCheck();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> attackOneAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> attackTwoAttacks;

	UAnimMontage* currentMontage;

	EAttackType currentAttackType;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite) // TODO: Complete this
	//TArray<FChargeAttackData> attackTwoAttacks;
};
