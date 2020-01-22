// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

//TODO: Move some headers from base character to player character
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

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	attackOneComboMaxIndex = attackOneAttacks.Num();
	attackTwoComboMaxIndex = attackTwoAttacks.Num();
	//UE_LOG(LogTemp, Warning, TEXT("Current attackOneComboMaxIndex is %d"), attackOneComboMaxIndex)
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	sprintTick(DeltaTime);
	regenStaminaTick(DeltaTime);
	standbyCheckTick();
}

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

	PlayerInputComponent->BindAction("ActionSprint", IE_Pressed, this, &APlayerCharacter::sprintPressed);
	PlayerInputComponent->BindAction("ActionSprint", IE_Released, this, &APlayerCharacter::sprintReleased);

	PlayerInputComponent->BindAction("AttackOne", IE_Pressed, this, &APlayerCharacter::attackOnePressed);
	PlayerInputComponent->BindAction("AttackOne", IE_Released, this, &APlayerCharacter::attackOneReleased);

	PlayerInputComponent->BindAction("AttackTwo", IE_Pressed, this, &APlayerCharacter::attackTwoPressed);
	PlayerInputComponent->BindAction("AttackTwo", IE_Released, this, &APlayerCharacter::attackTwoReleased);

	// Sprint attempt
}

void APlayerCharacter::attackOnePressed()
{
	if (canAttack()) // TODO warning: May need refinenement
	{
		// Inititate attack
		bChargeAttackStarted = true;
		currentAttackType = EAttackType::AttackOneCombo;
		windUpChargeAttack(attackOneAttacks[attackOneComboCurrentIndex]); // TODO: May need to secure
	}
}

void APlayerCharacter::attackOneReleased()
{
	if (bChargeAttackStarted && (currentAttackType == EAttackType::AttackOneCombo))
	{
		releaseAttack_Implementation();
	}
}

void APlayerCharacter::attackTwoPressed()
{
	if (canAttack()) // TODO warning: May need refinenement
	{
		if (attackTwoAttacks.IsValidIndex(attackTwoComboCurrentIndex)) // To check if attacks exist
		{
			// Inititate attack
			bChargeAttackStarted = true;
			currentAttackType = EAttackType::AttackTwoCombo;
			windUpChargeAttack(attackTwoAttacks[attackTwoComboCurrentIndex]); // TODO: May need to secure
		}
	}
}

void APlayerCharacter::attackTwoReleased()
{
	if (bChargeAttackStarted && (currentAttackType == EAttackType::AttackTwoCombo))
	{
		releaseAttack_Implementation();
	}
}

inline void APlayerCharacter::sprintPressed()
{
	bSprintingActive = true;
}

inline void APlayerCharacter::sprintReleased()
{
	bSprintingActive = false;
}

inline void APlayerCharacter::standbyCheckTick() // Tick function to check if player is in standby
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

void APlayerCharacter::sprintTick(float DeltaTime)
{
	// TODO: Refine this
	{
		if (canSprint() && (currentStaminaPoints > 1.f))
		{
			GetCharacterMovement()->MaxWalkSpeed = maxSprintSpeed; // Should lerp
			currentStaminaPoints -= sprintStaminaCost * DeltaTime;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed; // Should lerp
		}
	}
}

void APlayerCharacter::regenStaminaTick(float DeltaTime)
{
	// Stamina regen tick
	if ((currentStaminaPoints) < maxStaminaPoints) // TODO: Should be a different condition, i. e. bRegenerate, (no actions in use) 
	{
		if (canRegenerateStamina())
		{
			currentStaminaPoints += baseStaminaRegen * DeltaTime;
		}
	}
	else if (currentStaminaPoints > maxStaminaPoints)
	{
		//currentStaminaPoints = 100.f;
	}
}

// Tick function to check if player is in standby

inline bool APlayerCharacter::canSprint()
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

inline bool APlayerCharacter::canRegenerateStamina()
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

inline bool APlayerCharacter::canAttack()
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

inline void APlayerCharacter::windUpChargeAttack(FChargeAttackData & inAttack)
{
	// TODO: Complete this and rest of function
	currentMontage = inAttack.AttackAnimMontage;
	GetMesh()->GetAnimInstance()->Montage_Play(inAttack.AttackAnimMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);

	GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("windUp"));
	//GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("release"));


}

inline void APlayerCharacter::releaseAttack_Implementation()
{
	if (bChargeAttackStarted == true)
	{
		bChargeAttackStarted = false;
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("release"));
		incrementAttackCombo();
	}
}

inline float APlayerCharacter::getStaminaPoints_Implementation()
{
	return currentStaminaPoints;
}

inline void APlayerCharacter::setStaminaPoints_Implementation(float newStaminaPoints)
{
	currentStaminaPoints = newStaminaPoints;
}
