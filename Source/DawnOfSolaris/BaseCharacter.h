// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class DAWNOFSOLARIS_API ABaseCharacter : public ACharacter
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

	float maxHealthPoints{ 100 };
	float currentHealthPoints{ 100 };

	float maxStaminaPoints{ 100 };
	float currentStaminaPoints{ 100 };

	UFUNCTION(BlueprintCallable)
	float getHealthPoints();

	UFUNCTION(BlueprintCallable)
	void setHealthPoints(float newHealthPoints);

	UFUNCTION(BlueprintCallable)
	float getStaminaPoints();

	UFUNCTION(BlueprintCallable)
	void setStaminaPoints(float newHealthPoints);

};
