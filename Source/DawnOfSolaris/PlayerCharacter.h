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

	void attackOnePressed();


	float maxStaminaPoints{ 100 };
	float currentStaminaPoints{ 100 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FChargeAttackData> attackOneAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FChargeAttackData> attackTwoAttacks;

	void standbyCheck()
	{
		if (!bAttackActionActive && !bSelfHitstunActive && !bDodgingActive && !bSprintingActive)
		{
			bStandbyActive = true;
		}
		else
		{
			bStandbyActive = false;
		}
	}

	bool canSprint()
	{
		if (!bAttackActionActive && !bSelfHitstunActive && !bDodgingActive && bSprintingActive)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool canRegenerateStamina()
	{
		if (bStandbyActive)
		{
			return true; // TODO: Add delay before stamina starts to regenerate
		}
		else
		{
			return false;
		}
	}
	bool canAttack()
	{
		if (!bSelfHitstunActive && !bDodgingActive && !bSprintingActive)
		{
			if (!bChargeAttackStarted)
			{
				return true;
			}
			else
			{
				return false;
			}

		}
		else
		{
			return false;
		}

	}

	void windUpChargeAttack(FChargeAttackData& inAttack)
	{
		// TODO: Complete this and rest of function
		currentMontage = inAttack.AttackAnimMontage;
		GetMesh()->GetAnimInstance()->Montage_Play(inAttack.AttackAnimMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);

		GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("windUp"));
		//GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("release"));


	}
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
		void releaseAttack();
	virtual void releaseAttack_Implementation() override
	{
		if (bChargeAttackStarted == true)
		{
			bChargeAttackStarted = false;
			GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("release"));
			incrementAttackCombo();
		}
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
		float getStaminaPoints();
	virtual float getStaminaPoints_Implementation() override
	{
		return currentStaminaPoints;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
		void setStaminaPoints(float newStaminaPoints);
	virtual void setStaminaPoints_Implementation(float newStaminaPoints) override
	{
		currentStaminaPoints = newStaminaPoints;
	}

	float maxSprintSpeed{ 1100 };

	bool bDodgingActive{ false }; // Active in dodge frames
	bool bSprintingActive{ false }; // Active when sprint button is held	
	bool bChargeAttackStarted{ false };

	void sprintActivate()
	{
		bSprintingActive = true;
	}
	void sprintDeactivate()
	{
		bSprintingActive = false;
	}
};
