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
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
//#include "AIController.h"


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

	// set our turn rates for input
	BaseTurnRate = 60.f;
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

	// Set weapon
	Weapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon"));
	Weapon->SetChildActorClass(TSubclassOf<ABaseWeapon>());
	Weapon->SetupAttachment(GetMesh(), "hand_r");

	//Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "hand_r");
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (bUseCustomWeaponAttachSocket)
	{
		Weapon->SetRelativeLocation(FVector::ZeroVector);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(customWeaponAttachSocketName));		
	}

	defaultCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	launchedCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	maxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	maxRotationRate = GetCharacterMovement()->RotationRate.Yaw;

	currentMovementData = FMovementData(maxWalkSpeed, maxRotationRate);
	defaultMovementData = FMovementData(maxWalkSpeed, maxRotationRate);

	if (bUseRandomDefaultMovementSpeed)
	{
		maxWalkSpeed = FMath::RandRange(50.f, GetCharacterMovement()->MaxWalkSpeed);

		currentMovementData = FMovementData(maxWalkSpeed, maxRotationRate);
		defaultMovementData = FMovementData(maxWalkSpeed, maxRotationRate);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	movementSmoothingTick(DeltaTime);
	launchedTick(DeltaTime);
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

	PlayerInputComponent->BindAction("Action1", IE_Pressed, this, &ABaseCharacter::defaultAttackStartFromInputOne); //default attack one
	PlayerInputComponent->BindAction("Action2", IE_Pressed, this, &ABaseCharacter::defaultAttackStartFromInputTwo); //default attack two

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

void ABaseCharacter::movementSmoothingTick(float DeltaTime)
{
	//GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, 
	//currentMovementData.maxWalkSpeed, DeltaTime, currentMovementData.walkSpeedInterpSpeed);
	GetCharacterMovement()->RotationRate.Yaw = FMath::FInterpTo(GetCharacterMovement()->RotationRate.Yaw, 
		currentMovementData.maxRotationRate, DeltaTime, currentMovementData.rotInterpSpeed);

	GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, 
		currentMovementData.maxWalkSpeed, DeltaTime, maxWalkInterpRate);
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
	if (bIsGrounded || bIsLaunched)
	{
		nullifyMovement();
	}
	else if (bAttackActionActive)
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

void ABaseCharacter::nullifyMovement()
{
	GetCharacterMovement()->RotationRate.Yaw = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

//void ABaseCharacter::setMovementData(FMovementData inMovementData)
//{
//	defaultMovementData = inMovementData;
//
//	//GetCharacterMovement()->MaxWalkSpeed = inMovementData.maxWalkSpeed;
//	//GetCharacterMovement()->RotationRate.Yaw = inMovementData.maxRotationRate;
//}

bool ABaseCharacter::getIsAttacking_Implementation()
{
	return bAttackActionActive;
}

bool ABaseCharacter::getIsDefeated_Implementation()
{
	return bIsDefeated;
}

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

	if (!bIsDefeated) // TODO: Change if statement to disable overlap events instead
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
			bIsDefeated = true;
			currentHealthPoints = 0;
			UE_LOG(LogTemp, Warning, TEXT("%s is defeated"), *this->GetName());
			startIsDefeatedProcedure();
		}
	}
}

void ABaseCharacter::attackStart_Implementation()
{
	if (currentDefaultAttackData.AttackHitbox == EAttackHitboxType::Default && Weapon->GetChildActor())
	{
		Execute_sendAttackDataToWeapon(Weapon->GetChildActor(), currentDefaultAttackData, CombatAlignment, this);
	}

	bAttackActionActive = true; // TODO(?): Uncomment if commented out
	bCanCancelAction = false;

	updateMovement();
	//UE_LOG(LogTemp, Warning, TEXT("Attack start"))
}

void ABaseCharacter::attackEnd_Implementation()
{
	bAttackActionActive = false;

	GetMesh()->GetAnimInstance()->Montage_Stop(0.15f, currentMontage);

	updateMovement();

	if (currentDefaultAttackData.AttackHitbox == EAttackHitboxType::Default && Weapon->GetChildActor())
	{
		Execute_deactivateAttackHitbox(Weapon->GetChildActor());
	}
	//UE_LOG(LogTemp, Warning, TEXT("Attack end"))
}

void ABaseCharacter::activateAttackHitbox_Implementation()
{
	if (currentDefaultAttackData.AttackHitbox == EAttackHitboxType::Default && Weapon->GetChildActor())
	{
		Execute_activateAttackHitbox(Weapon->GetChildActor());
	}
}

void ABaseCharacter::deactivateAttackHitbox_Implementation()
{
	if (currentDefaultAttackData.AttackHitbox == EAttackHitboxType::Default && Weapon->GetChildActor())
	{
		Execute_deactivateAttackHitbox(Weapon->GetChildActor());
	}
}

void ABaseCharacter::fireProjectile_Implementation()
{
	if (currentDefaultAttackData.AttackHitbox == EAttackHitboxType::Default && Weapon->GetChildActor())
	{
		Execute_fireProjectile(Weapon->GetChildActor());
	}
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

void ABaseCharacter::defaultAttackStartFromInputOne()
{
	defaultAttackStart();
}

void ABaseCharacter::defaultAttackStartFromInputTwo()
{
	defaultAttackStart(1);
}

void ABaseCharacter::defaultAttackStart(int attackIndex)
{
	bDefaultAttackStarted = true;
	
	//Ensure hitboxes are disabled at start of attack
	Execute_deactivateAttackHitbox(this);

	if (defaultAttacks.IsValidIndex(attackIndex))  // To check if attacks exist
	{
		currentDefaultAttackData = defaultAttacks[attackIndex];
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

	if (currentDefaultAttackData.AttackHitbox == EAttackHitboxType::Default && Weapon->GetChildActor())
	{
		Execute_deactivateAttackHitbox(Weapon->GetChildActor());
	}
}

bool ABaseCharacter::startDefaultAttack_Implementation(int index)
{
	if (!bSelfHitstunActive)
	{
		defaultAttackStart(index);
		return true;
	}
	else
	{
		return false;
	}	
}

bool ABaseCharacter::startRandomDefaultAttack_Implementation()
{
	if (!bSelfHitstunActive)
	{
		int randomAttackNum = FMath::RandRange(0, (defaultAttacks.Num() - 1));
		defaultAttackStart(randomAttackNum);

		return true;
	}
	else
	{
		return false;
	}
}

void ABaseCharacter::startHitstun_Implementation()
{
	bSelfHitstunActive = true;
	currentMovementData = hitstunMovementData;

	// TODO: Lerp this
	FVector tempDirection = currentReceivedAttackData.hitDirection * -1; // Sets rotation to follow direction
	SetActorRotation(FRotator(0.f, tempDirection.ToOrientationRotator().Yaw, 0.f));
	// TODO: Set slide friction on

	debugFunctionForBlueprint(); //// TODO: Delete
}

void ABaseCharacter::endHitstun_Implementation()
{
	currentMovementData = defaultMovementData;
	bSelfHitstunActive = false;
	// TODO: Set slide friction off

	hitstunReset();
	updateMovement();
}

// Hitstun calculation: hitstun < 0.1f: hitstunAnimationOnly, 0.1f - 0.3f: hitstunFlinch, 0.3f - 0.7f: hitstunFlinchWithKnockback, > 0.7f: hitstunLaunched
void ABaseCharacter::runHitstunProcedure(float inHitstunStrengthReceived, FVector hitDirection)
{
	//UE_LOG(LogTemp, Warning, TEXT("Start hitstun procedure"))
	if (inHitstunStrengthReceived <= 0.1f)
	{
		if (IsValid(hitstunAnimations.hitstunLightAnimMontage))
		{
			//currentMontage = hitstunAnimations.hitstunLightAnimMontage;
			//GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		}
	}
	else if (inHitstunStrengthReceived > 0.1f && inHitstunStrengthReceived <= 0.3f)
	{
		if (IsValid(hitstunAnimations.hitstunLightAnimMontage))
		{
			currentMontage = hitstunAnimations.hitstunHeavyAnimMontage;
			GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
			hitstunReset();
		}
		// TODO: Make stun timer
	}
	else if (inHitstunStrengthReceived > 0.3f && inHitstunStrengthReceived <= 0.7f)
	{		
		if (IsValid(hitstunAnimations.hitstunHeavyAnimMontage))
		{
			currentMontage = hitstunAnimations.hitstunHeavyAnimMontage;
			GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
			hitstunReset();
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
		//UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *adjustedDirection.ToString())
		GetMesh()->GetAnimInstance()->Montage_Stop(0.15f, currentMontage);
		hitstunReset();

		Execute_startHitstun(this);
		nullifyMovement();

		//GetCapsuleComponent()->SetSimulatePhysics(true);
		//GetWorldTimerManager().SetTimer(physicsTimerHandle, this, &ABaseCharacter::endPhysics, 0.5f); 
		//GetCapsuleComponent()->AddImpulse(adjustedDirection*300);
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);

		if (GetController()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			Execute_pauseAI(GetController());
		}		
		
		LaunchCharacter(adjustedDirection, true, false);
		//GetCharacterMovement()->AddImpulse(adjustedDirection, true);

		//debugVector(FVector(hitDirection.X, hitDirection.Y, 0).GetSafeNormal());

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
	//GetCapsuleComponent()->SetCapsuleHalfHeight(launchedCapsuleHalfHeight);
	bIsLaunched = true;
}

void ABaseCharacter::endLaunch()
{
	//GetCapsuleComponent()->SetCapsuleHalfHeight(defaultCapsuleHalfHeight);

	//GetCapsuleComponent()->SetSimulatePhysics(false);
	bIsLaunched = false;

	if (GetController()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		Execute_resumeAI(GetController());
	}
	

	startGrounded();
}

void ABaseCharacter::startGrounded()
{
	bIsGrounded = true;
	updateMovement();

	GetWorldTimerManager().SetTimer(launchedTimerHandle, this, &ABaseCharacter::endGrounded, 2.f); // TODO: Make timer amount random
}

void ABaseCharacter::endGrounded()
{
	bIsGrounded = false;

	updateMovement();
	Execute_endHitstun(this);
}

void ABaseCharacter::endPhysics()
{
	GetCapsuleComponent()->SetSimulatePhysics(false);
}

void ABaseCharacter::hitstunReset()
{
	bAttackActionActive = false;
	bStandbyActive = true;
	bDefaultAttackStarted = false;
	bCanCancelAction = false;

	if (Weapon->GetChildActor())
	{
		Execute_deactivateAttackHitbox(this);
	}
	
	updateMovement();
}

void ABaseCharacter::startIsDefeatedProcedure()
{
	// TODO: Add more to this
	GetCapsuleComponent()->SetEnableGravity(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OuterCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
	GetCharacterMovement()->DisableMovement();

	Execute_attackEnd(this);
	//Weapon->DetachFromParent(true);
	if (Weapon->GetChildActor()->IsValidLowLevelFast())
	{
		Execute_detachWeapon(Weapon->GetChildActor());
	}
	

	eventIsDefeated();
}


