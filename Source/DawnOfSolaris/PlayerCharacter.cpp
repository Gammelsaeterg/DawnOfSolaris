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
#include "Components/PrimitiveComponent.h"

APlayerCharacter::APlayerCharacter()
{

	// TODO: Needs refactoring
	LeftHandHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftHandHitbox"));
	LeftHandHitbox->SetupAttachment(GetMesh(), FName("hand_l"));
	LeftHandHitbox->SetCapsuleSize(20.f, 20.f, true);
	LeftHandHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBeginLeftHandHitbox);
	LeftHandHitbox->SetGenerateOverlapEvents(false);

	RightHandHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightHandHitbox"));
	RightHandHitbox->SetupAttachment(GetMesh(), FName("hand_r"));
	RightHandHitbox->SetCapsuleSize(20.f, 20.f, true);
	RightHandHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBeginRightHandHitbox);
	RightHandHitbox->SetGenerateOverlapEvents(false);

	LeftFootHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftFootHitbox"));
	LeftFootHitbox->SetupAttachment(GetMesh(), FName("foot_l"));
	LeftFootHitbox->SetCapsuleSize(20.f, 20.f, true);
	LeftFootHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBeginLeftFootHitbox);
	LeftFootHitbox->SetGenerateOverlapEvents(false);

	RightFootHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightFootHitbox"));
	RightFootHitbox->SetupAttachment(GetMesh(), FName("foot_r"));
	RightFootHitbox->SetCapsuleSize(20.f, 20.f, true);
	RightFootHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBeginRightFootHitbox);
	RightFootHitbox->SetGenerateOverlapEvents(false);

	//TODO: Research if these overlap events can be bound to one function
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
		currentAttackHitboxType = attackOneAttacks[attackOneComboCurrentIndex].AttackHitbox;

		currentAttackDataToSend.damageAmount = attackOneAttacks[attackOneComboCurrentIndex].minDamageValue; // TODO: Get charge value
		currentAttackDataToSend.hitstunStrength = attackOneAttacks[attackOneComboCurrentIndex].minHitstunValue; // TODO: Get charge value
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
			currentAttackHitboxType = attackTwoAttacks[attackOneComboCurrentIndex].AttackHitbox;

			currentAttackDataToSend.damageAmount = attackTwoAttacks[attackTwoComboCurrentIndex].minDamageValue; // TODO: Get charge value
			currentAttackDataToSend.hitstunStrength = attackTwoAttacks[attackTwoComboCurrentIndex].minHitstunValue; // TODO: Get charge value
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

	bAttackHitboxActive = false; // Disable old hitbox for new attack
	clearHitActors();
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

void APlayerCharacter::OnOverlapBeginLeftHandHitbox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
												    UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, 
													bool bFromSweep, const FHitResult & SweepResult)
{
	OnOverlapBeginAttackHit(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void APlayerCharacter::OnOverlapBeginRightHandHitbox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
												     UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, 
													 bool bFromSweep, const FHitResult & SweepResult)
{
	OnOverlapBeginAttackHit(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void APlayerCharacter::OnOverlapBeginLeftFootHitbox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
												    UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, 
													bool bFromSweep, const FHitResult & SweepResult)
{
	OnOverlapBeginAttackHit(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void APlayerCharacter::OnOverlapBeginRightFootHitbox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
												     UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, 
													 bool bFromSweep, const FHitResult & SweepResult)
{
	OnOverlapBeginAttackHit(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void APlayerCharacter::OnOverlapBeginAttackHit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
											   UPrimitiveComponent * OtherComp, int32 OtherBodyIndex,
											   bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (!(isActorAlreadyHit(OtherActor)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Overlapped actor: %s"), *OtherActor->GetName());
			UE_LOG(LogTemp, Warning, TEXT("Overlapped self comp: %s"), *OverlappedComp->GetName());
			UE_LOG(LogTemp, Warning, TEXT("Overlapped self hitbox: %s"), *(GetEnumValueAsString<EAttackHitboxType>("EAttackHitboxType", currentAttackHitboxType)));
			hitActors.Add(OtherActor);

			// TODO: On other actor take damage, set charge value multiplier to damage and hitstun strength sent
			ICharacterInterface* characterInterface = Cast<ICharacterInterface>(OtherActor);
			if (characterInterface)
			{
				FVector hitDirection;
				hitDirection = OverlappedComp->GetPhysicsLinearVelocity().GetSafeNormal(0.000001f); 

				if (hitDirection.Size() < 1) // If getting physics velocity fails
				{
					hitDirection = GetMesh()->GetRightVector().GetSafeNormal(0.000001f);
				}

				characterInterface->Execute_takeDamage(OtherActor, currentAttackDataToSend.damageAmount, 
													   hitDirection, FVector(0.f, 0.f, 0.f), 
					                                   this, currentAttackDataToSend.hitstunStrength);
			}			
		}
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

void APlayerCharacter::activateAttackHitbox_Implementation()
{
	enableHitbox(currentAttackHitboxType, true);
	bAttackHitboxActive = true;
}

void APlayerCharacter::deactivateAttackHitbox_Implementation()
{
	disableAllHitboxes();
	bAttackHitboxActive = false;

	clearHitActors();
}

void APlayerCharacter::enableHitbox(EAttackHitboxType inHitbox, bool enabled)
{
	UPrimitiveComponent* overlapCollisionToEnable;

	switch (inHitbox)
	{
	case EAttackHitboxType::LeftHand:
		overlapCollisionToEnable = LeftHandHitbox;
		break;
	case EAttackHitboxType::RightHand:
		overlapCollisionToEnable = RightHandHitbox;
		break;
	case EAttackHitboxType::LeftFoot:
		overlapCollisionToEnable = LeftFootHitbox;
		break;
	case EAttackHitboxType::RightFoot:
		overlapCollisionToEnable = RightFootHitbox;
		break;
	default:
		overlapCollisionToEnable = LeftHandHitbox; // TODO: Default if all fails, change this!
	}

	if (overlapCollisionToEnable)
	{
		overlapCollisionToEnable->SetGenerateOverlapEvents(enabled);
		overlapCollisionToEnable->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void APlayerCharacter::disableAllHitboxes()
{
	// TODO: Make this into an array and refactor
	LeftHandHitbox->SetGenerateOverlapEvents(false);
	RightHandHitbox->SetGenerateOverlapEvents(false);
	LeftFootHitbox->SetGenerateOverlapEvents(false);
	RightFootHitbox->SetGenerateOverlapEvents(false);

	LeftHandHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftFootHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightFootHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool APlayerCharacter::isActorAlreadyHit(AActor * inActor)
{
	// TODO: Simplify this
	bool bIsActorAlreadyHit{ false };
	for (AActor * hitActor : hitActors)
	{
		if (inActor == hitActor)
		{
			bIsActorAlreadyHit = true;
		}
	}

	if (bIsActorAlreadyHit)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void APlayerCharacter::clearHitActors()
{
	hitActors.Empty();
}
