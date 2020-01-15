// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"


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
	
	attackOneComboMaxIndex = attackOneAttacks.Num();
	attackTwoComboMaxIndex = attackTwoAttacks.Num();
	//UE_LOG(LogTemp, Warning, TEXT("Current attackOneComboMaxIndex is %d"), attackOneComboMaxIndex)
}

void ABaseCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	// Check if standby
	standbyCheck();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABaseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABaseCharacter::LookUpAtRate);

	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("ActionSprint", IE_Pressed, this, &ABaseCharacter::sprintActivate);
	PlayerInputComponent->BindAction("ActionSprint", IE_Released, this, &ABaseCharacter::sprintDeactivate);

	PlayerInputComponent->BindAction("AttackOne", IE_Pressed, this, &ABaseCharacter::attackOnePressed);
	PlayerInputComponent->BindAction("AttackOne", IE_Released, this, &ABaseCharacter::attackOneReleased);

	PlayerInputComponent->BindAction("AttackTwo", IE_Pressed, this, &ABaseCharacter::attackTwoPressed);
	PlayerInputComponent->BindAction("AttackTwo", IE_Released, this, &ABaseCharacter::attackTwoReleased);

	// Sprint attempt
}

void ABaseCharacter::incrementAttackCombo()
{
	// TODO: This should be switch statements

	if (currentAttackType == EAttackType::AttackOneCombo)
	{
		if ((attackOneComboCurrentIndex + 1) >= attackOneComboMaxIndex)
		{
			attackOneComboCurrentIndex = 0;
		}
		else
		{
			++attackOneComboCurrentIndex;
		}
	}
	else if (currentAttackType == EAttackType::AttackTwoCombo)
	{
		if ((attackTwoComboCurrentIndex + 1) >= attackTwoComboMaxIndex)
		{
			attackTwoComboCurrentIndex = 0;
		}
		else
		{
			++attackTwoComboCurrentIndex;
		}
	}
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

void ABaseCharacter::attackOnePressed()
{
	if (canAttack()) // TODO warning: May need refinenement
	{
		// Inititate attack
		bChargeAttackStarted = true;
		currentAttackType = EAttackType::AttackOneCombo;
		windUpChargeAttack(attackOneAttacks[attackOneComboCurrentIndex]); // TODO: May need to secure
	}
}

void ABaseCharacter::attackOneReleased()
{
	if (bChargeAttackStarted && (currentAttackType == EAttackType::AttackOneCombo))
	{
		releaseAttack_Implementation();
	}	
}

void ABaseCharacter::attackTwoPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("Attack one pressed"))
	if (canAttack()) // TODO warning: May need refinenement
	{
		// Inititate attack
		bChargeAttackStarted = true;
		currentAttackType = EAttackType::AttackTwoCombo;
		windUpChargeAttack(attackTwoAttacks[attackTwoComboCurrentIndex]); // TODO: May need to secure
	}
}

void ABaseCharacter::attackTwoReleased()
{
	if (bChargeAttackStarted && (currentAttackType == EAttackType::AttackTwoCombo))
	{
		releaseAttack_Implementation();
	}
}

bool ABaseCharacter::canSprint()
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

bool ABaseCharacter::canRegenerateStamina()
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

bool ABaseCharacter::canAttack()
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

void ABaseCharacter::windUpChargeAttack(FChargeAttackData & inAttack)
{
	// TODO: Complete this and rest of function
	currentMontage = inAttack.AttackAnimMontage;
	GetMesh()->GetAnimInstance()->Montage_Play(inAttack.AttackAnimMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true); 
	
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("windUp"));
	//GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("release"));
	
		
}

void ABaseCharacter::releaseAttack_Implementation()
{
	if (bChargeAttackStarted == true)
	{
		bChargeAttackStarted = false;
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("release"));
		incrementAttackCombo();
	}
}

void ABaseCharacter::standbyCheck()
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

