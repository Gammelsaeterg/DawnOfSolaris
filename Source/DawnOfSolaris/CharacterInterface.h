// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DawnOfSolaris.h"
#include "CharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DAWNOFSOLARIS_API ICharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getHealthPoints();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getMaxHealthPoints();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setHealthPoints(float newHealthPoints);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getStaminaPoints();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getMaxStaminaPoints();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setStaminaPoints(float newStaminaPoints);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	void setChargeAmount(float newChargeAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsHitstunned();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsLaunched();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsGrounded();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsAttacking();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void sendAttackDataToWeapon(FDefaultAttackData inAttackData, ECombatAlignment inCombatAlignment, ABaseCharacter* inOwner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	float getWindUpChargeAmount();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void minimumChargeAmountReached();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	bool getIsWindingUpChargeAttack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void windUpStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void releaseStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void releaseEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void canCancelAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void activateAttackHitbox();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void deactivateAttackHitbox();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void fireProjectile();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void takeDamage(FAttackData inAttackData);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void attackStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void attackEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void startHitstun();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void endHitstun();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void startDodge();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void endDodge();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void startDefaultAttack(int index);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	ECombatAlignment getAlignment();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector interact(class AActor* interactorActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void setInteractableObjectInRange(class AInteractableObject* inObject);
};
