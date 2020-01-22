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
	//Current input component

	//Inputs
	void attackOnePressed();
	void attackOneReleased();

	void attackTwoPressed();
	void attackTwoReleased();

	void grabAttackPressed(); //TODO: Complete function
	void grabAttackReleased(); //TODO: Complete function

	void sprintPressed();
	void sprintReleased();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxStaminaPoints{ 100 };
	float currentStaminaPoints{ 100 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxSprintSpeed{ 1100 };

	bool bDodgingActive{ false }; // Active in dodge frames
	bool bSprintingActive{ false }; // Active when sprint button is held	
	bool bChargeAttackStarted{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> attackOneAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> attackTwoAttacks;


	void standbyCheck(); // Tick function to check if player is in standby
	bool canSprint();
	bool canRegenerateStamina();
	bool canAttack();

	void windUpChargeAttack(FChargeAttackData& inAttack);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void releaseAttack();
	virtual void releaseAttack_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getStaminaPoints();
	virtual float getStaminaPoints_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setStaminaPoints(float newStaminaPoints);
	virtual void setStaminaPoints_Implementation(float newStaminaPoints) override;




};
