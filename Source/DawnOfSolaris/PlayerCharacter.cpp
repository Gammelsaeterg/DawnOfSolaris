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
#include "TimerManager.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // Needed to make blueprint tick work // TODO: Remove when blueprint tick no longer is needed

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
	
	defaultComboOneMaxIndex = defaultComboOneAttacks.Num();
	defaultComboTwoMaxIndex = defaultComboTwoAttacks.Num();

	updateComboMaxIndexes();
	updateCurrentIndexes();
	//UE_LOG(LogTemp, Warning, TEXT("Current defaultComboOneComboMaxIndex is %d"), defaultComboOneComboMaxIndex)
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

	PlayerInputComponent->BindAction("Action1", IE_Pressed, this, &APlayerCharacter::Action1Pressed);
	PlayerInputComponent->BindAction("Action1", IE_Released, this, &APlayerCharacter::Action1Released);

	PlayerInputComponent->BindAction("Action2", IE_Pressed, this, &APlayerCharacter::Action2Pressed);
	PlayerInputComponent->BindAction("Action2", IE_Released, this, &APlayerCharacter::Action2Released);

	// Sprint attempt
}

void APlayerCharacter::comboAttackPressed(EActionType inActionType)
{
	//if (bChargeAttackStarted) // This if statement is for queueing charge inputs while doing charge attack combos
	//{
	//	if (inActionType == currentActionType)
	//	{
	//		bChargeAttackInputHeld = true;
	//	}
	//}
	//else 
		if (canAttack() && (inActionType == EActionType::DefaultComboOne || inActionType == EActionType::DefaultComboTwo))
	{
		if (getCurrentMoveset(inActionType).IsValidIndex(currentComboIndexes[(uint8)inActionType]))
		{
			// Inititate attack
			bChargeAttackStarted = true;
			currentActionType = inActionType;
			windUpChargeAttack(getCurrentMoveset(inActionType)[currentComboIndexes[(uint8)inActionType]]);
			currentAttackHitboxType = getCurrentMoveset(inActionType)[currentComboIndexes[(uint8)inActionType]].AttackHitbox;

			bChargeAttackInputHeld = true;
		}
	}
	else if (inActionType == currentActionType) // This if statement is also for queueing inputs while doing charge attack combos
	{
		bChargeAttackInputHeld = true;
	}
	else
	{
		queuedActionTypes.Push(inActionType); // For queuing up other actions // TODO: Collapse to addActionToQueue function
	}
}

void APlayerCharacter::comboAttackReleased(EActionType inActionType)
{
	if (bChargeAttackStarted && (currentActionType == inActionType) && bMinimumChargeReached)
	{		
		releaseStart_Implementation();
		bChargeAttackInputHeld = false;
	}
	else
	{
		if (inActionType == currentActionType)
		{
			bChargeAttackInputHeld = false;
		}
		else // TODO: Collapse to removeQueuedActionFromQueue function
		{
			if (queuedActionTypes.Num() > 0) // These if statements and for loops is for removing a queue action if button is released
			{
				for (int i = 0; i < queuedActionTypes.Num(); ++i)
				{
					if (inActionType == queuedActionTypes[i])
					{
						queuedActionTypes.RemoveAt(i);
					}
				}
			}
		}
	}
}

void APlayerCharacter::updateComboMaxIndexes()
{
	maxComboIndexes.Empty();
	maxComboIndexes.Reserve(2); // TODO(?) Update number to max movesets
	maxComboIndexes.Push(getCurrentMoveset(EActionType::DefaultComboOne).Num());
	maxComboIndexes.Push(getCurrentMoveset(EActionType::DefaultComboTwo).Num());
}

void APlayerCharacter::updateCurrentIndexes()
{
	currentComboIndexes.Empty();
	for (int NumOfMovesets = 2; NumOfMovesets > 0; --NumOfMovesets) // TODO(?) Update elements to number of max movesets, 2 is default for now
	{
		currentComboIndexes.Push(0); 
	}	
}

void APlayerCharacter::Action0Pressed()
{
	actionPressed(Action0Input);
}

void APlayerCharacter::Action1Pressed()
{
	actionPressed(Action1Input);
}

void APlayerCharacter::Action2Pressed()
{
	actionPressed(Action2Input);
}

void APlayerCharacter::Action3Pressed()
{
	actionPressed(Action3Input);
}

void APlayerCharacter::Action0Released()
{
	actionReleased(Action0Input);
}

void APlayerCharacter::Action1Released()
{
	actionReleased(Action1Input);
}

void APlayerCharacter::Action2Released()
{
	actionReleased(Action2Input);
}

void APlayerCharacter::Action3Released()
{
	actionReleased(Action3Input);
}

void APlayerCharacter::actionPressed(EActionType inActionType)
{
	switch (inActionType)
	{
	case EActionType::DefaultComboOne:
		comboAttackPressed(inActionType);
		break;
	case EActionType::DefaultComboTwo:
		comboAttackPressed(inActionType);
		break;
	default:
		break;
	}
}

void APlayerCharacter::actionReleased(EActionType inActionType)
{
	switch (inActionType)
	{
	case EActionType::DefaultComboOne:
		comboAttackReleased(inActionType);;
		break;
	case EActionType::DefaultComboTwo:
		comboAttackReleased(inActionType);;
		break;
	default:
		break;
	}
}

void APlayerCharacter::actionReleased()
{
	if (currentActionType == EActionType::DefaultComboOne || currentActionType == EActionType::DefaultComboTwo)
	{
		comboAttackReleased(currentActionType);
	}
}

void APlayerCharacter::incrementAttackCombo(EActionType inActionType)
{
	if (inActionType == EActionType::DefaultComboOne || inActionType == EActionType::DefaultComboTwo)
	{
		if ((currentComboIndexes[(uint8)inActionType] + 1) >= maxComboIndexes[(uint8)inActionType])
		{
			currentComboIndexes[(uint8)inActionType] = 0;
		}
		else
		{
			++currentComboIndexes[(uint8)inActionType];
		}
	}
}

TArray<FChargeAttackData> APlayerCharacter::getCurrentMoveset(EActionType inActionType, int inMovesetIndex)
{
	if (inActionType == EActionType::DefaultComboOne || inActionType == EActionType::DefaultComboTwo)
	{

		switch (inActionType)
		{
		case EActionType::DefaultComboOne:
			return defaultComboOneAttacks;
			break;
		case EActionType::DefaultComboTwo:
			return defaultComboTwoAttacks;
			break;
		default:
			return TArray<FChargeAttackData>(); // Should not happen
			break;
		}
	}
	else // TODO(?): Return inMovesetIndex
	{
		return TArray<FChargeAttackData>(); // Should not happen
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

void APlayerCharacter::attackAI(EActionType inAttackCombo, float chargeAmount)
{
	if (inAttackCombo == EActionType::DefaultComboOne || inAttackCombo == EActionType::DefaultComboTwo)
	{
		currentActionType = inAttackCombo;
		actionPressed(currentActionType);
		GetWorldTimerManager().SetTimer(attackTimerAI, this, &APlayerCharacter::actionReleased, chargeAmount);
	}

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
	// TODO: Refine this, also should lerp
	{
		if (!bSelfHitstunActive)
		{
			if (canSprint() && (currentStaminaPoints > 1.f))
			{
				currentMovementData.maxWalkSpeed = maxSprintSpeed; // Should lerp
				if (GetVelocity().Size() > maxWalkSpeed)
				{
					currentStaminaPoints -= sprintStaminaCost * DeltaTime;
				}
				updateMovement();
			}
			else
			{
				currentMovementData.maxWalkSpeed = maxWalkSpeed; // Should lerp
				updateMovement();
			}
		}
	}
	// more TODO: Only use this for lerp, the speed change itself should be an event
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
	if (bCanCancelAction)
	{
		return true;
	}
	else if (!bSelfHitstunActive && !bDodgingActive && !bSprintingActive && !bAttackActionActive)
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

	ABaseCharacter::attackStart();
}

inline void APlayerCharacter::releaseStart_Implementation()
{
	if (bChargeAttackStarted == true)
	{
		// Sets current montage pos as charge amount // TODO(?): May be a better way to get position
		Execute_setChargeAmount(this, GetMesh()->GetAnimInstance()->Montage_GetPosition(currentMontage));
		//UE_LOG(LogTemp, Warning, TEXT("Took hitstunValue: %f"), GetMesh()->GetAnimInstance()->Montage_GetPosition(currentMontage));

		bChargeAttackStarted = false;
		bMinimumChargeReached = false;

		GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("release"));
		incrementAttackCombo(currentActionType);
	}
}

void APlayerCharacter::releaseEnd_Implementation()
{
	if (bChargeAttackInputHeld)
	{
		//if (currentActionType == EActionType::DefaultComboOne || currentActionType == EActionType::DefaultComboTwo) // TODO(?): May not be needed
		//{
			comboAttackPressed(currentActionType); // Will do button pressed procedure since the button is still held at this point
		//}		
	}
	else
	{
		if (queuedActionTypes.Num() > 0) // Check if there are actions in queue
		{
			actionPressed(queuedActionTypes.Top());
			queuedActionTypes.Empty();
		}
	}
}

FAttackData APlayerCharacter::calculateChargeAttackValues(FChargeAttackData inChargeAttackData)
{
	//TODO: Refactor, also needs rounding/deadzones for currentCharge amount (i.e. if close to 0 or 1, round down or up)

	FAttackData tempChargeAttackData{ FAttackData() };

	float minDamage = inChargeAttackData.minDamageValue;
	float maxDamage = inChargeAttackData.maxDamageValue;

	float minHitstunValue = inChargeAttackData.minHitstunValue;
	float maxHitstunValue = inChargeAttackData.maxHitstunValue;

	float damageDifference = maxDamage - minDamage;
	float hitstunValueDifference = maxHitstunValue - minHitstunValue;

	float damageToSend = minDamage + (currentChargeAmount * damageDifference);
	float hitstunValueToSend = minHitstunValue + (currentChargeAmount * hitstunValueDifference);

	tempChargeAttackData.damageAmount = damageToSend;
	tempChargeAttackData.hitstunStrength = hitstunValueToSend;
	return tempChargeAttackData;
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
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) // Default nullptr and self check // (OtherComp->IsA(USkeletalMesh::StaticClass()))
	{
		ICharacterInterface* characterInterface = Cast<ICharacterInterface>(OtherActor);
		if (characterInterface && !(isActorAlreadyHit(OtherActor))) // Check if actor has interface and has not already been hit
		{
			if (canDamageInteract(CombatAlignment, characterInterface->Execute_getAlignment(OtherActor))) // Check if can damage interact
			{
				//UE_LOG(LogTemp, Warning, TEXT("Overlapped actor: %s"), *OtherActor->GetName()); //// Debug texts, very nice and valuable 
				UE_LOG(LogTemp, Warning, TEXT("Overlapped other comp: %s"), *OtherComp->GetName());
				//UE_LOG(LogTemp, Warning, TEXT("Overlapped self hitbox: %s"), *(GetEnumValueAsString<EAttackHitboxType>("EAttackHitboxType", currentAttackHitboxType)));
				//UE_LOG(LogTemp, Warning, TEXT("Other alignment is: %s"), *(GetEnumValueAsString<ECombatAlignment>("ECombatAlignment", characterInterface->Execute_getAlignment(OtherActor))));
				hitActors.Add(OtherActor);

				// TODO: On other actor take damage, set charge value multiplier to damage and hitstun strength sent			

				FVector hitDirection;
				hitDirection = OverlappedComp->GetPhysicsLinearVelocity().GetSafeNormal(0.000001f);
				if (hitDirection.Size() < 1) // If getting physics velocity fails
				{
					hitDirection = GetMesh()->GetRightVector().GetSafeNormal(0.000001f);
				}

				if ((currentActionType == EActionType::DefaultComboOne || currentActionType == EActionType::DefaultComboTwo))
				{
					FAttackData tempAttackData; // TODO: Refactor
					tempAttackData = calculateChargeAttackValues(getCurrentMoveset(currentActionType)[currentComboIndexes[(uint8)currentActionType]]);

					currentAttackDataToSend = FAttackData(tempAttackData.damageAmount,
					hitDirection, SweepResult.Location,
					this, tempAttackData.hitstunStrength);
				}
				else
				{
					currentAttackDataToSend = FAttackData(currentAttackDataToSend.damageAmount,
					hitDirection, SweepResult.Location,
					this, currentAttackDataToSend.hitstunStrength);
				}

				characterInterface->Execute_takeDamage(OtherActor, currentAttackDataToSend);
			}								
		}
	}
}

void APlayerCharacter::takeDamage_Implementation(FAttackData inAttackData)
{
	Super::takeDamage_Implementation(inAttackData);
	//UE_LOG(LogTemp, Warning, TEXT("Take damage player char: %s"), *this->GetName());
}

void APlayerCharacter::startHitstun_Implementation()
{
	Super::startHitstun_Implementation();
}

void APlayerCharacter::endHitstun_Implementation()
{
	Super::endHitstun_Implementation();
}

inline float APlayerCharacter::getStaminaPoints_Implementation()
{
	return currentStaminaPoints;
}

float APlayerCharacter::getMaxStaminaPoints_Implementation()
{
	return maxStaminaPoints;
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

void APlayerCharacter::setChargeAmount_Implementation(float newChargeAmount)
{
	currentChargeAmount = newChargeAmount;
}

bool APlayerCharacter::getIsWindingUpChargeAttack_Implementation()
{
	return bChargeAttackStarted;
}

void APlayerCharacter::minimumChargeAmountReached_Implementation()
{
	if (!bChargeAttackInputHeld)
	{
		releaseStart_Implementation();
	}
	else
	{
		bMinimumChargeReached = true;
	}
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
