// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Debug function, TODO: DELETE
	{
		if (bSprintingActive && (currentStaminaPoints > 1.f)) // TODO: Should be a different condition, i. e. bCanSprint etc.
		{
			GetCharacterMovement()->MaxWalkSpeed = maxSprintSpeed; // Should lerp
			currentStaminaPoints -= sprintStaminaCost * DeltaTime;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed; // Should lerp
		}
	}

	// Stamina regen tick
	if ((currentStaminaPoints) < maxStaminaPoints) // TODO: Should be a different condition, i. e. bRegenerate, (no actions in use) 
	{
		currentStaminaPoints += baseStaminaRegen * DeltaTime;
	}
	else if (currentStaminaPoints > maxStaminaPoints)
	{
		//currentStaminaPoints = 100.f;
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("ActionSprint", IE_Pressed, this, &ABaseCharacter::sprintActivate);
	PlayerInputComponent->BindAction("ActionSprint", IE_Released, this, &ABaseCharacter::sprintDeactivate);

	// Sprint attempt
}

float ABaseCharacter::getHealthPoints_Implementation()
{
	return currentHealthPoints;
}

void ABaseCharacter::setHealthPoints_Implementation(float newHealthPoints)
{
	currentHealthPoints = newHealthPoints;

}

float ABaseCharacter::getStaminaPoints_Implementation()
{
	return currentStaminaPoints;
}

void ABaseCharacter::setStaminaPoints_Implementation(float newStaminaPoints)
{
	currentStaminaPoints = newStaminaPoints;
}

void ABaseCharacter::sprintActivate()
{
	bSprintingActive = true;
}

void ABaseCharacter::sprintDeactivate()
{
	bSprintingActive = false;
}

