// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

//May not need every of these
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

APlayerCharacter::APlayerCharacter()
{

}


//void APlayerCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	// TODO: Refine this
//	{
//		if (canSprint() && (currentStaminaPoints > 1.f))
//		{
//			GetCharacterMovement()->MaxWalkSpeed = maxSprintSpeed; // Should lerp
//			currentStaminaPoints -= sprintStaminaCost * DeltaTime;
//		}
//		else
//		{
//			GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed; // Should lerp
//		}
//	}
//
//	// Stamina regen tick
//	if ((currentStaminaPoints) < maxStaminaPoints) // TODO: Should be a different condition, i. e. bRegenerate, (no actions in use) 
//	{
//		if (canRegenerateStamina())
//		{
//			currentStaminaPoints += baseStaminaRegen * DeltaTime;
//		}
//	}
//	else if (currentStaminaPoints > maxStaminaPoints)
//	{
//		//currentStaminaPoints = 100.f;
//	}
//
//	// Check if standby
//	standbyCheck();
//}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABaseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABaseCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("ActionSprint", IE_Pressed, this, &ABaseCharacter::sprintActivate);
	PlayerInputComponent->BindAction("ActionSprint", IE_Released, this, &ABaseCharacter::sprintDeactivate);

	PlayerInputComponent->BindAction("AttackOne", IE_Pressed, this, &APlayerCharacter::attackOnePressed);
	PlayerInputComponent->BindAction("AttackOne", IE_Released, this, &ABaseCharacter::attackOneReleased);

	PlayerInputComponent->BindAction("AttackTwo", IE_Pressed, this, &ABaseCharacter::attackTwoPressed);
	PlayerInputComponent->BindAction("AttackTwo", IE_Released, this, &ABaseCharacter::attackTwoReleased);

	// Sprint attempt
}

void APlayerCharacter::attackOnePressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Derived function"))
}
