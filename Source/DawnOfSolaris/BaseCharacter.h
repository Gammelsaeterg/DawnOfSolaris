// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterInterface.h"
#include "Components/InputComponent.h"

#include "BaseCharacter.generated.h"

UCLASS()
class DAWNOFSOLARIS_API ABaseCharacter : public ACharacter, public ICharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	bool canSprint();
	bool canRegenerateStamina();

	void standbyCheck();
};
