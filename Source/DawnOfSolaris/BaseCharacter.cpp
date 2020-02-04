// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"



// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Default 3rd person template variables

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	OuterCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("OuterCapsuleComponent"));
	OuterCapsuleComponent->InitCapsuleSize(45.f, 100.0f);
	OuterCapsuleComponent->SetupAttachment(RootComponent);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// TODO(?)
	//defaultComboOneComboMaxIndex = defaultComboOneAttacks.Num();
	//defaultComboTwoComboMaxIndex = defaultComboTwoAttacks.Num();
	//UE_LOG(LogTemp, Warning, TEXT("Current defaultComboOneComboMaxIndex is %d"), defaultComboOneComboMaxIndex)
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
}

void ABaseCharacter::updateMovement()
{
	// TODO: Lerp this
	if (bAttackActionActive)
	{
		setMovementData(combatMovementData);
	}
	else
	{
		setMovementData(currentMovementData);		
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABaseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABaseCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Action1", IE_Pressed, this, &ABaseCharacter::defaultAttackStartFromInput); //default attack

	//PlayerInputComponent->BindAction("ActionSprint", IE_Pressed, this, &ABaseCharacter::sprintActivate);
	//PlayerInputComponent->BindAction("ActionSprint", IE_Released, this, &ABaseCharacter::sprintDeactivate);

	//PlayerInputComponent->BindAction("defaultComboOne", IE_Pressed, this, &ABaseCharacter::defaultComboOnePressed);
	//PlayerInputComponent->BindAction("defaultComboOne", IE_Released, this, &ABaseCharacter::defaultComboOneReleased);

	//PlayerInputComponent->BindAction("defaultComboTwo", IE_Pressed, this, &ABaseCharacter::defaultComboTwoPressed);
	//PlayerInputComponent->BindAction("defaultComboTwo", IE_Released, this, &ABaseCharacter::defaultComboTwoReleased);

	// Sprint attempt
}

void ABaseCharacter::setMovementData(FMovementData inMovementData)
{
	GetCharacterMovement()->MaxWalkSpeed = inMovementData.maxWalkSpeed;
	GetCharacterMovement()->RotationRate.Yaw = inMovementData.maxRotationRate;
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, inMovementData.maxRotationRate);
}

float ABaseCharacter::getHealthPoints_Implementation()
{
	return currentHealthPoints;
}

float ABaseCharacter::getMaxHealthPoints_Implementation()
{
	return maxHealthPoints;

}

void ABaseCharacter::setHealthPoints_Implementation(float newHealthPoints)
{
	currentHealthPoints = newHealthPoints;
}

void ABaseCharacter::takeDamage_Implementation(FAttackData inAttackData)
{
	//UE_LOG(LogTemp, Warning, TEXT("Take damage base char: %s"), *this->GetName());

	if (!bIsDeafeated) // TODO: Change if statement to disable overlap events instead
	{		
		if (currentHealthPoints > inAttackData.damageAmount)
		{
			currentHealthPoints -= inAttackData.damageAmount;
			UE_LOG(LogTemp, Warning, TEXT("Took damage: %f, health left: %f"), inAttackData.damageAmount, currentHealthPoints);

			//Hitstun launch
			//FVector adjustedDirection = (FVector(inAttackData.hitDirection.X, inAttackData.hitDirection.Y, 0.f).GetSafeNormal()) * 1000;
			//LaunchCharacter(adjustedDirection, false, false);

			//Hitstun animation
			//if ()
			//{

			//}
		}
		else
		{
			bIsDeafeated = true;
			currentHealthPoints = 0;
			UE_LOG(LogTemp, Warning, TEXT("%s is defeated"), *this->GetName());
			startIsDefeatedProcedure();
		}
	}
}

void ABaseCharacter::attackStart_Implementation()
{
	bAttackActionActive = true;

	updateMovement();
	//UE_LOG(LogTemp, Warning, TEXT("Attack start"))
}

void ABaseCharacter::attackEnd_Implementation()
{
	bAttackActionActive = false;

	updateMovement();
}

ECombatAlignment ABaseCharacter::getAlignment_Implementation()
{
	return CombatAlignment;
}

void ABaseCharacter::defaultAttackStartFromInput()
{
	defaultAttackStart();
}

void ABaseCharacter::defaultAttackStart(int attackIndex)
{
	bDefaultAttackStarted = true;

	if (defaultAttacks.IsValidIndex(attackIndex))  // To check if attacks exist
	{
		currentMontage = defaultAttacks[attackIndex].AttackAnimMontage;
		if (GetMesh()->GetAnimInstance() != nullptr)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		}
	}

}

void ABaseCharacter::defaultAttackEnd()
{
	bDefaultAttackStarted = false;
}

void ABaseCharacter::startHitstun_Implementation()
{
	bSelfHitstunActive = true;
	currentMovementData = hitstunMovementData;

	// TODO: Lerp this
	FVector tempDirection = currentReceivedAttackData.hitDirection * -1; // Sets rotation to follow direction
	SetActorRotation(FRotator(0.f, tempDirection.ToOrientationRotator().Yaw, 0.f));

	updateMovement();

	debugFunctionForBlueprint(); //// TODO: Delete
}

void ABaseCharacter::endHitstun_Implementation()
{
	currentMovementData = defaultMovementData;
	bSelfHitstunActive = false;

	updateMovement();
}

void ABaseCharacter::startIsDefeatedProcedure()
{
	// TODO: Add more to this
	GetCapsuleComponent()->SetEnableGravity(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OuterCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetCharacterMovement()->DisableMovement();
}


