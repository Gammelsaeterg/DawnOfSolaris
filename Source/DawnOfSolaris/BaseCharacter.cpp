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
#include "Math/UnrealMathUtility.h"
#include "Components/ChildActorComponent.h"
#include "BaseWeapon.h"


// Sets default values
ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Default 3rd person template variables

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	OuterCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("OuterCapsuleComponent"));
	OuterCapsuleComponent->InitCapsuleSize(45.f, 100.0f);
	OuterCapsuleComponent->SetupAttachment(RootComponent);

	CurrentHeldWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon"));
	CurrentHeldWeapon->SetChildActorClass(TSubclassOf<ABaseWeapon>());
	CurrentHeldWeapon->SetupAttachment(GetMesh(), "hand_r");

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

	// Reserve space for button queue for opimalization
	queuedActionTypes.Reserve(20); // TODO(?): 20 inputs at the same time may be overkill, reduce later

	// Can rotate character during root motion
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	defaultCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	launchedCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	//CurrentWeaponHeld = GetWorld()->SpawnActor<ABaseWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	//CurrentWeaponHeld->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "hand_r");

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

	movementSmoothingTick(DeltaTime);
	launchedTick(DeltaTime);
}

void ABaseCharacter::movementSmoothingTick(float DeltaTime)
{
	//GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, 
	//currentMovementData.maxWalkSpeed, DeltaTime, currentMovementData.walkSpeedInterpSpeed);
	GetCharacterMovement()->RotationRate.Yaw = FMath::FInterpTo(GetCharacterMovement()->RotationRate.Yaw, 
		currentMovementData.maxRotationRate, DeltaTime, currentMovementData.rotInterpSpeed);

	GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, 
		currentMovementData.maxWalkSpeed, DeltaTime, 4.f);
}

void ABaseCharacter::launchedTick(float DeltaTime)
{
	if (bIsLaunched)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			endLaunch();
		}
	}
}

void ABaseCharacter::updateMovement()
{
	// TODO: Lerp this
	if (bAttackActionActive)
	{
		//setMovementData(combatMovementData);
		currentMovementData = combatMovementData;
		GetCharacterMovement()->bUseSeparateBrakingFriction = false;
	}
	else if (bSelfHitstunActive)
	{
		currentMovementData = hitstunMovementData;
		GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	}
	else
	{
		currentMovementData = defaultMovementData;
		GetCharacterMovement()->bUseSeparateBrakingFriction = false;
	}

	//Experimental functions below // TODO(?): Delete if necessary
	//GetCharacterMovement()->RootMotionParams;
	//GetCharacterMovement()->AnimRootMotionVelocity = 2.f;

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

//void ABaseCharacter::setMovementData(FMovementData inMovementData)
//{
//	defaultMovementData = inMovementData;
//
//	//GetCharacterMovement()->MaxWalkSpeed = inMovementData.maxWalkSpeed;
//	//GetCharacterMovement()->RotationRate.Yaw = inMovementData.maxRotationRate;
//}

bool ABaseCharacter::getIsHitstunned_Implementation()
{
	return bSelfHitstunActive;
}

bool ABaseCharacter::getIsLaunched_Implementation()
{
	return bIsLaunched;
}

bool ABaseCharacter::getIsGrounded_Implementation()
{
	return bIsGrounded;
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
			UE_LOG(LogTemp, Warning, TEXT("Took damage: %f, took hitstunValue: %f, health left: %f"), inAttackData.damageAmount, inAttackData.hitstunStrength, currentHealthPoints);

			currentReceivedAttackData = inAttackData; // Saves data so hitstun event can get correct values	
			// Hitstun procedures
			runHitstunProcedure(currentReceivedAttackData.hitstunStrength, currentReceivedAttackData.hitDirection);
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
	bAttackActionActive = true; // TODO: Uncomment
	bCanCancelAction = false;

	updateMovement();
	//UE_LOG(LogTemp, Warning, TEXT("Attack start"))
}

void ABaseCharacter::attackEnd_Implementation()
{
	bAttackActionActive = false;

	updateMovement();
	//UE_LOG(LogTemp, Warning, TEXT("Attack end"))
}

void ABaseCharacter::canCancelAction_Implementation()
{
	bCanCancelAction = true;
	defaultMovementData.maxRotationRate = 540.f; // TODO: Get a different variable to adjust this

	debugDespawnFX();
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
	// TODO: Set slide friction on

	updateMovement();

	debugFunctionForBlueprint(); //// TODO: Delete
}

void ABaseCharacter::endHitstun_Implementation()
{
	currentMovementData = defaultMovementData;
	bSelfHitstunActive = false;
	// TODO: Set slide friction off

	updateMovement();
}

// Hitstun calculation: hitstun < 0.1f: hitstunAnimationOnly, 0.1f - 0.3f: hitstunFlinch, 0.3f - 0.7f: hitstunFlinchWithKnockback, > 0.7f: hitstunLaunched
void ABaseCharacter::runHitstunProcedure(float inHitstunStrengthReceived, FVector hitDirection)
{
	//UE_LOG(LogTemp, Warning, TEXT("Start hitstun procedure"))
	if (inHitstunStrengthReceived < 0.1f)
	{
		if (IsValid(hitstunAnimations.hitstunLightAnimMontage))
		{
			currentMontage = hitstunAnimations.hitstunLightAnimMontage;
			GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		}
	}
	else if (inHitstunStrengthReceived > 0.1f && inHitstunStrengthReceived < 0.3f)
	{
		if (IsValid(hitstunAnimations.hitstunLightAnimMontage))
		{
			currentMontage = hitstunAnimations.hitstunLightAnimMontage;
			GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		}
		// TODO: Make stun timer
	}
	else if (inHitstunStrengthReceived > 0.3f && inHitstunStrengthReceived < 0.7f)
	{		
		if (IsValid(hitstunAnimations.hitstunHeavyAnimMontage))
		{
			currentMontage = hitstunAnimations.hitstunHeavyAnimMontage;
			GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		}
		// TODO: Make stun timer

		//Knockback handling
		FVector adjustedDirection = (FVector(hitDirection.X, hitDirection.Y, 0.f).GetSafeNormal()) * calculateKnockbackLength(inHitstunStrengthReceived);
		LaunchCharacter(adjustedDirection, false, false);
		//UE_LOG(LogTemp, Warning, TEXT("Start hitstun procedure: %s"), *adjustedDirection.ToString())
	}
	else if (inHitstunStrengthReceived > 0.7f)
	{
		// TODO: Make stun timer and launch character in air
		float tempLaunchZaxis{ 500.f }; // TODO: Make this a variable in the header file or dynamic compared to hitstunstrength

		FVector adjustedDirection = (FVector(hitDirection.X, hitDirection.Y, 0).GetSafeNormal()) * calculateLaunchLength(inHitstunStrengthReceived) + FVector(0.f, 0.f, tempLaunchZaxis);
		LaunchCharacter(adjustedDirection, false, false);

		startLaunch();
	}
	else
	{
		// Debug else, function should normally not reach this line
		UE_LOG(LogTemp, Warning, TEXT("You dun goofed"))
	}
}

void ABaseCharacter::startLaunch()
{
	Execute_startHitstun(this);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(launchedCapsuleHalfHeight);
	
	bIsLaunched = true;
}

void ABaseCharacter::endLaunch()
{
	//GetCapsuleComponent()->SetCapsuleHalfHeight(defaultCapsuleHalfHeight);
	Execute_endHitstun(this);

	bIsLaunched = false;
	Execute_endHitstun(this);
}

void ABaseCharacter::startIsDefeatedProcedure()
{
	// TODO: Add more to this
	GetCapsuleComponent()->SetEnableGravity(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OuterCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
	GetCharacterMovement()->DisableMovement();
}


