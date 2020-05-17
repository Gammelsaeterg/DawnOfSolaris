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
#include "Math/UnrealMathUtility.h"
#include "InteractableObject.h"
#include "BaseWeapon.h"


APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	
	PrimaryActorTick.bCanEverTick = true; // Needed to make blueprint tick work // TODO: Remove when blueprint tick no longer is needed

	// TODO: Needs refactoring
	LeftHandHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftHandHitbox"));
	LeftHandHitbox->SetupAttachment(GetMesh(), FName("hand_l"));
	LeftHandHitbox->SetCapsuleSize(20.f, 20.f, true);
	LeftHandHitbox->SetGenerateOverlapEvents(false);

	RightHandHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightHandHitbox"));
	RightHandHitbox->SetupAttachment(GetMesh(), FName("hand_r"));
	RightHandHitbox->SetCapsuleSize(20.f, 20.f, true);
	RightHandHitbox->SetGenerateOverlapEvents(false);

	LeftFootHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftFootHitbox"));
	LeftFootHitbox->SetupAttachment(GetMesh(), FName("foot_l"));
	LeftFootHitbox->SetCapsuleSize(20.f, 20.f, true);
	LeftFootHitbox->SetGenerateOverlapEvents(false);

	RightFootHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightFootHitbox"));
	RightFootHitbox->SetupAttachment(GetMesh(), FName("foot_r"));
	RightFootHitbox->SetCapsuleSize(20.f, 20.f, true);
	RightFootHitbox->SetGenerateOverlapEvents(false);

	LeftKneeHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftKneeHitbox"));
	LeftKneeHitbox->SetupAttachment(GetMesh(), FName("calf_l"));
	LeftKneeHitbox->SetCapsuleSize(20.f, 20.f, true);
	LeftKneeHitbox->SetGenerateOverlapEvents(false);

	RightKneeHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightKneeHitbox"));
	RightKneeHitbox->SetupAttachment(GetMesh(), FName("calf_r"));
	RightKneeHitbox->SetCapsuleSize(20.f, 20.f, true);
	RightKneeHitbox->SetGenerateOverlapEvents(false);

	//Weapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon"));
	//Weapon->SetChildActorClass(TSubclassOf<ABaseWeapon>());
	Weapon->SetupAttachment(GetMesh(), "lowerarm_l");
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlayerCharacterMovementComponent = Cast<UPlayerCharacterMovementComponent>(Super::GetMovementComponent());
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("Player character BeginPlay()"))
	//Weapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon"));
	//Weapon->SetChildActorClass(TSubclassOf<ABaseWeapon>());

	//Weapon->SetupAttachment(GetMesh(), "lowerarm_r");
	//Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("lowerarm_r"));

	LeftHandHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBeginAttackHit);
	RightHandHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBeginAttackHit);
	LeftFootHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBeginAttackHit);
	LeftKneeHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBeginAttackHit);
	RightKneeHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBeginAttackHit);

	
	defaultComboOneMaxIndex = defaultComboOneAttacks.Num(); // TODO(?): Investigate whether or not this is necessary
	defaultComboTwoMaxIndex = defaultComboTwoAttacks.Num(); // TODO(?): Investigate whether or not this is necessary

	if (combatMovesets.Num() > 0)
	{
		unlockedCombatMovesets.Add(combatMovesets[0]);
	}

	// Sets first moveset in moveset array as default moveset // TODO(?): This comment chain may be redundant
	if (unlockedCombatMovesets.Num() > 0)
	{
		if (unlockedCombatMovesets.IsValidIndex(0))
		{
			setMoveset(&unlockedCombatMovesets[0]); 
			currentMovesetIndex = 0; // TODO(?): May not be necessary as it is already initialized as 0;
		}
	}

	if (startWithAllUpgrades)
	{
		Execute_unlockMoveset(this, 2);
		Execute_unlockMoveset(this, 1);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	sprintTick(DeltaTime);
	regenStaminaTick(DeltaTime);
	regenHealthTick(DeltaTime);
	standbyCheckTick();
	windUpChargeAmountTick(DeltaTime);
	interactableTick(DeltaTime);
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

	PlayerInputComponent->BindAction("Action0", IE_Pressed, this, &APlayerCharacter::Action0Pressed);
	PlayerInputComponent->BindAction("Action0", IE_Released, this, &APlayerCharacter::Action0Released);

	PlayerInputComponent->BindAction("Action1", IE_Pressed, this, &APlayerCharacter::Action1Pressed);
	PlayerInputComponent->BindAction("Action1", IE_Released, this, &APlayerCharacter::Action1Released);

	PlayerInputComponent->BindAction("Action2", IE_Pressed, this, &APlayerCharacter::Action2Pressed);
	PlayerInputComponent->BindAction("Action2", IE_Released, this, &APlayerCharacter::Action2Released);

	PlayerInputComponent->BindAction("Action3", IE_Pressed, this, &APlayerCharacter::Action3Pressed);
	PlayerInputComponent->BindAction("Action3", IE_Released, this, &APlayerCharacter::Action3Released);

	PlayerInputComponent->BindAction("BrowseUp", IE_Pressed, this, &APlayerCharacter::BrowseUpPressed);
	PlayerInputComponent->BindAction("BrowseDown", IE_Pressed, this, &APlayerCharacter::BrowseDownPressed);
}

void APlayerCharacter::comboAttackPressed(EActionType inActionType)
{	
	if (bCanSprintAttack)
	{
		sprintAttack(inActionType);
	}
	else if (canAttack() && (inActionType == EActionType::DefaultComboOne || inActionType == EActionType::DefaultComboTwo))
	{
		if (getCurrentComboAttacks(inActionType).IsValidIndex(currentComboIndexes[(uint8)inActionType]))
		{
			// Inititate attack
			if (bIsTargeting)
			{
				updateMovement();
				SetActorRotation(FRotator(0.f, targetRotationDirection.Yaw, 0.f));
			}

			bChargeAttackStarted = true;
			currentActionType = inActionType;
			windUpChargeAttack(getCurrentComboAttacks(inActionType)[currentComboIndexes[(uint8)inActionType]]);
			currentAttackHitboxType = getCurrentComboAttacks(inActionType)[currentComboIndexes[(uint8)inActionType]].AttackHitbox;
			currentChargeAttackStaminaConsumptionRate = getCurrentComboAttacks(inActionType)[currentComboIndexes[(uint8)inActionType]].baseStaminaConsumptionRate;

			bChargeAttackInputHeld = true;
		}
	}
	else
	{
		bQueuedInputHeld = true;
		lastQueuedActionHeld = inActionType; // Remember queued input 
		queuedActionTypes.Push(inActionType); // For queuing up other actions // TODO: Collapse to addActionToQueue function
		//UE_LOG(LogTemp, Warning, TEXT("Attack queued"))
	}
}

void APlayerCharacter::comboAttackReleased(EActionType inActionType)
{
	if (bChargeAttackStarted && (currentActionType == inActionType) && bMinimumChargeReached)
	{		
		releaseStart_Implementation();
		bChargeAttackInputHeld = false;
	}
	else if (inActionType == currentActionType)
	{
		bChargeAttackInputHeld = false;
	}

	if (currentActionType != inActionType) // TODO: Collapse to removeQueuedActionFromQueue function
	{
		if (queuedActionTypes.Num() > 0 && !bQueueAttacks) // These if statements and for loops is for removing a queue action if button is released
		{
			for (int i = 0; i < queuedActionTypes.Num(); ++i)
			{
				if (inActionType == queuedActionTypes[i])
				{
					queuedActionTypes.RemoveAt(i, 1, true);
				}
			}
		}
	}

	if (inActionType == lastQueuedActionHeld)
	{
		bQueuedInputHeld = false;
	}
}

void APlayerCharacter::updateComboMaxIndexes()
{
	maxComboIndexes.Empty();
	maxComboIndexes.Reserve(2); // TODO(?) Update number to max movesets
	maxComboIndexes.Push(getCurrentComboAttacks(EActionType::DefaultComboOne).Num());
	maxComboIndexes.Push(getCurrentComboAttacks(EActionType::DefaultComboTwo).Num());
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

void APlayerCharacter::BrowseUpPressed()
{
	startMovesetChange(true);
	//findNextMoveset(true);
}

void APlayerCharacter::BrowseDownPressed()
{
	startMovesetChange(false);
	//findPreviousMoveset(true);
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
	case EActionType::DodgeRoll:
		startDodgeRoll();
		break;
	case EActionType::Interact:
		interact();
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

void APlayerCharacter::resetAttackCombos()
{
	for (int i = 0; i < currentComboIndexes.Num(); ++i)
	{
		currentComboIndexes[i] = 0;
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

void APlayerCharacter::startDodgeRoll()
{
	if (canDodge())
	{
		if (currentStaminaPoints > 23.f) // TODO: Set stamina cost as variable
		{
			currentStaminaPoints -= 23.f;

			currentMontage = currentDodgeRollData.dodgeAnimMontage;
			GetPlayerCharacterMovementComponent()->setRootMotionVelocityMultiplier(1.f * currentDodgeRollData.dodgeRootAnimationMultiplier);
			GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		}
	}
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

FMovesetData APlayerCharacter::getCurrentMovesetFromPlayer_Implementation()
{
	return *currentMovesetData;
}

FMovesetData APlayerCharacter::getNextMovesetFromPlayer_Implementation()
{
	return *findNextMoveset(false);
}

FMovesetData APlayerCharacter::getPreviousMovesetFromPlayer_Implementation()
{
	return *findPreviousMoveset(false);
}

bool APlayerCharacter::movesetsUnlocked_Implementation()
{
	if (unlockedCombatMovesets.Num() > 1)
	{
		return true;
	}
	else
	{
		return false;
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
	// TODO: Refine this, also should lerp // TODO update: May be in order now
	{
		if (!bSelfHitstunActive)
		{
			if (canSprint() && (currentStaminaPoints > 1.f))
			{
				//defaultMovementData.maxWalkSpeed = 1100.f;
				defaultMovementData.maxWalkSpeed = FMath::FInterpTo(defaultMovementData.maxWalkSpeed, maxSprintSpeed, DeltaTime, 5.f);
				defaultMovementData.maxRotationRate = FMath::FInterpTo(defaultMovementData.maxRotationRate, maxSprintRotationRate, DeltaTime, 5.f);
				if (GetVelocity().Size() > maxWalkSpeed + (maxSprintSpeed / 10.f))
				{
					currentStaminaPoints -= sprintStaminaCost * DeltaTime;
					bCanSprintAttack = true;
				}
				else
				{
					bCanSprintAttack = false;
				}
			}
			else
			{
				//defaultMovementData.maxWalkSpeed = 600.f;
				defaultMovementData.maxWalkSpeed = FMath::FInterpTo(defaultMovementData.maxWalkSpeed, maxWalkSpeed, DeltaTime, 5.f);
				defaultMovementData.maxRotationRate = FMath::FInterpTo(defaultMovementData.maxRotationRate, maxRotationRate, DeltaTime, 5.f);
				bCanSprintAttack = false;
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
		currentStaminaPoints = 100.f;
	}
}


void APlayerCharacter::regenHealthTick(float DeltaTime)
{
	// Health regen tick
	if ((currentHealthPoints < maxHealthPoints) && !bIsDefeated) // TODO: Should be a different condition, i. e. bRegenerate, (no actions in use) 
	{

		currentHealthPoints += 1.f * DeltaTime;
		
	}
	else if (currentHealthPoints > maxHealthPoints)
	{
		currentHealthPoints = maxHealthPoints;
	}
}


TArray<FChargeAttackData> APlayerCharacter::getCurrentComboAttacks(EActionType inActionType, int inComboIndex)
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
	else // TODO(?): Return inComboIndex
	{
		return TArray<FChargeAttackData>(); // Should not happen
	}
}

void APlayerCharacter::setMoveset(FMovesetData* inMovesetData)
{
	// TODO: Add more nullptr/IsValid() checks before assigning movesets
	//eventMovesetChanged();
	currentMovesetData = inMovesetData;

	if (currentMovesetData->playerWeaponMorph)
	{
		Weapon->SetChildActorClass(currentMovesetData->playerWeaponMorph);
	}
	else
	{
		Weapon->DestroyChildActor();
	}

	//Clear old attack combos // TODO(?): May not be necessary
	defaultComboOneAttacks.Empty();
	defaultComboTwoAttacks.Empty();

	defaultComboOneAttacks = inMovesetData->ChargeAttacksOne;
	defaultComboTwoAttacks = inMovesetData->ChargeAttacksTwo;

	sprintAttackOne = inMovesetData->SprintAttackOne;
	sprintAttackTwo = inMovesetData->SprintAttackTwo;

	//SetAnimClass(inMovesetData->movesetAnimBlueprint->GetAnimBlueprintGeneratedClass()); // TODO: This crashes the build, fix or remove for later

	updateComboMaxIndexes();
	updateCurrentIndexes();
}

FMovesetData* APlayerCharacter::findNextMoveset(bool setNewMoveset)
{
	if (unlockedCombatMovesets.Num() > 1) // Checks if there are at least two movesets
	{
		if ((currentMovesetIndex + 1) < unlockedCombatMovesets.Num())
		{
			if (setNewMoveset)
			{
				currentMovesetIndex += 1;
				eventMovesetChanged();

				setMoveset(&unlockedCombatMovesets[currentMovesetIndex]);			
				return &unlockedCombatMovesets[currentMovesetIndex];
			}
			else
			{
				return &unlockedCombatMovesets[currentMovesetIndex + 1];
			}
		}
		else
		{
			if (setNewMoveset)
			{
				setMoveset(&unlockedCombatMovesets[0]);
				currentMovesetIndex = 0;
				eventMovesetChanged();

				return &unlockedCombatMovesets[0];
			}
			else
			{
				return &unlockedCombatMovesets[0];
			}

		}
	}
	else
	{
		return &unlockedCombatMovesets[0];
	}
}

FMovesetData* APlayerCharacter::findPreviousMoveset(bool setNewMoveset)
{
	if (unlockedCombatMovesets.Num() > 1) // Checks if there are at least two movesets
	{
		if (currentMovesetIndex < 1)
		{
			if (setNewMoveset)
			{
				currentMovesetIndex = unlockedCombatMovesets.Num() - 1;
				eventMovesetChanged();

				setMoveset(&unlockedCombatMovesets[currentMovesetIndex]);
				return &unlockedCombatMovesets[currentMovesetIndex];
			}
			else
			{
				return &unlockedCombatMovesets[unlockedCombatMovesets.Num() - 1];
			}

		}
		else
		{
			if (setNewMoveset)
			{
				currentMovesetIndex -= 1;
				eventMovesetChanged();

				setMoveset(&unlockedCombatMovesets[currentMovesetIndex]);
				return &unlockedCombatMovesets[currentMovesetIndex];
			}
			else
			{
				return &unlockedCombatMovesets[currentMovesetIndex - 1];
			}

		}
	}
	else
	{
		return &unlockedCombatMovesets[0];
	}
}

FMovesetData APlayerCharacter::getNextMoveset()
{
	FMovesetData* tempFMovementData;
	tempFMovementData = findNextMoveset(false);

	return *tempFMovementData;
}


FMovesetData APlayerCharacter::getPreviousMoveset()
{
	FMovesetData* tempFMovementData;
	tempFMovementData = findPreviousMoveset(false);

	return *tempFMovementData;
}

void APlayerCharacter::windUpChargeAmountTick(float deltaTime)
{	
	if (bChargeAttackStarted)
	{
		tickWindUpChargeAmount = GetMesh()->GetAnimInstance()->Montage_GetPosition(currentMontage);

		if (currentStaminaPoints > currentChargeAttackStaminaConsumptionRate * deltaTime)
		{
			currentStaminaPoints -= currentChargeAttackStaminaConsumptionRate * deltaTime;
		}
	}
}

void APlayerCharacter::interactableTick(float deltaTime)
{
	if (currentInteractableObject != nullptr)
	{
		if (!IsOverlappingActor(currentInteractableObject))
		{
			bInteractableObjectInRange = false;
			currentInteractableObject = nullptr;
		}
	}
}


// Tick function to check if player is in standby

inline bool APlayerCharacter::canSprint()
{
	updateMovement(); // TODO(?): May be a cheaper way to do this
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
	if (GetCharacterMovement()->IsFalling() || bIsDefeated)
	{
		return false;
	}
	else if (bIsLaunched || bIsGrounded)
	{
		return false;
	}
	else if (bCanCancelAction && !bDodgingActive) // TODO(?): Make it so you can cancel dodge into attack
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

bool APlayerCharacter::canDodge()
{
	if (bIsDefeated)
	{
		return false;
	}
	else if (bIsLaunched || bIsGrounded)
	{
		return false;
	}
	else if (!bSelfHitstunActive && !bDodgingActive && !bAttackActionActive)
	{
		if (!bChargeAttackStarted)
		{
			return true;
		}
		else
		{
			if (bCanCancelAction)
			{
				return true;
			}
			else
			{
				return false;
			}			
		}
	}
	else
	{
		return false;
	}
}


void APlayerCharacter::unlockMoveset_Implementation(int movesetIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("%d"))
	if (combatMovesets.IsValidIndex(movesetIndex))
	{
		unlockedCombatMovesets.Add(combatMovesets[movesetIndex]);
		eventCrystalAdded();
	}
}

inline void APlayerCharacter::windUpChargeAttack(FChargeAttackData & inAttack)
{
	//ABaseCharacter::attackStart();

	// TODO: Complete this and rest of function
	currentMontage = inAttack.AttackAnimMontage;
	float tempChargeAttackTimePlayRate = 1.f / inAttack.maxChargeTime;
	GetMesh()->GetAnimInstance()->Montage_Play(inAttack.AttackAnimMontage, tempChargeAttackTimePlayRate, EMontagePlayReturnType::MontageLength, 0.f, true);

	GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("windUp"));
	currentRootAnimationMultiplier = inAttack.rootMotionMultiplier;

	bAttackHitboxActive = false; // Disable old hitbox for new attack
	clearHitActors();

	if (inAttack.bUnstoppable)
	{
		PlayerCharacterMovementComponent->bIgnoreVerticalHit = true;
	}
}

void APlayerCharacter::windUpStart_Implementation()
{
	debugSpawnFX();
}

inline void APlayerCharacter::releaseStart_Implementation()
{
	if (bChargeAttackStarted == true)
	{
		if (bIsTargeting)
		{
			SetActorRotation(FRotator(0.f, targetRotationDirection.Yaw, 0.f));
		}

		GetPlayerCharacterMovementComponent()->resetThresholdHit();
		// Sets current montage pos as charge amount // TODO(?): May be a better way to get position
		Execute_setChargeAmount(this, GetMesh()->GetAnimInstance()->Montage_GetPosition(currentMontage));
		GetMesh()->GetAnimInstance()->Montage_SetPlayRate(currentMontage, 1.f);
		//UE_LOG(LogTemp, Warning, TEXT("Took hitstunValue: %f"), GetMesh()->GetAnimInstance()->Montage_GetPosition(currentMontage));

		GetPlayerCharacterMovementComponent()->setRootMotionVelocityMultiplier(currentChargeAmount * (currentRootAnimationMultiplier)); 

		bChargeAttackStarted = false;
		bMinimumChargeReached = false;

		GetMesh()->GetAnimInstance()->Montage_SetPlayRate(currentMontage, 1.f);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("release"));

		currentChargeAttackDataToSend = getCurrentComboAttacks(currentActionType)[currentComboIndexes[(uint8)currentActionType]];
		incrementAttackCombo(currentActionType);
	}
}

void APlayerCharacter::releaseEnd_Implementation()
{	
	PlayerCharacterMovementComponent->bIgnoreVerticalHit = false;
	if (bChargeAttackInputHeld)
	{
		queuedActionTypes.Empty();
		comboAttackPressed(currentActionType); // Will do button pressed procedure since the button is still held at this point
	}
	else
	{
		if (queuedActionTypes.Num() > 0) // Check if there are actions in queue
		{			
			actionPressed(queuedActionTypes.Top());			
			queuedActionTypes.Empty();					

			if (!bQueuedInputHeld)
			{
				bChargeAttackInputHeld = false;
			}
		}
		else
		{
			resetAttackCombos(); 
		}
	}
}

void APlayerCharacter::attackEnd_Implementation()
{
	PlayerCharacterMovementComponent->bIgnoreVerticalHit = false;
	Super::attackEnd_Implementation();
}

void APlayerCharacter::canCancelAction_Implementation()
{
	Super::canCancelAction_Implementation();

	if (bChargeAttackInputHeld)
	{
		queuedActionTypes.Empty();
		comboAttackPressed(currentActionType); // Will do button pressed procedure since the button is still held at this point
	}
	else
	{
		if (queuedActionTypes.Num() > 0) // Check if there are actions in queue
		{
			actionPressed(queuedActionTypes.Top());
			queuedActionTypes.Empty();

			if (!bQueuedInputHeld)
			{
				bChargeAttackInputHeld = false;
			}
		}
	}
}

void APlayerCharacter::sprintAttack(EActionType inActionType) // TODO(?) Refactor this function
{

	if (inActionType == EActionType::DefaultComboOne || inActionType == EActionType::DefaultComboTwo) // TODO(?): May not be necessary
	{
		currentChargeAttackDataToSend.projectileScaleMultiplier = 1.f; // Reset scale for sprint projectile attacks

		if (inActionType == EActionType::DefaultComboOne)
		{
			currentActionType = EActionType::SprintAttackOne;
			// Do sprint attack one here 
			UE_LOG(LogTemp, Warning, TEXT("Do sprint attack one here"));
			PlayerCharacterMovementComponent->bIgnoreVerticalHit = true;
			if (sprintAttackOne.sprintAttackAnimMontage->IsValidLowLevelFast())
			{
				currentMontage = sprintAttackOne.sprintAttackAnimMontage;
				currentAttackHitboxType = sprintAttackOne.AttackHitbox;

				currentAttackDataToSend.damageAmount = sprintAttackOne.damageValue;
				currentAttackDataToSend.hitstunStrength = sprintAttackOne.hitstunValue;

				GetPlayerCharacterMovementComponent()->setRootMotionVelocityMultiplier(1.f * sprintAttackOne.rootMotionMultiplier);
				GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
			}
		}
		else if (inActionType == EActionType::DefaultComboTwo)
		{
			currentActionType = EActionType::SprintAttackTwo;
			// Do sprint attack two here
			UE_LOG(LogTemp, Warning, TEXT("Do sprint attack two here"));
			PlayerCharacterMovementComponent->bIgnoreVerticalHit = true;
			if (sprintAttackTwo.sprintAttackAnimMontage->IsValidLowLevelFast())
			{
				currentMontage = sprintAttackTwo.sprintAttackAnimMontage;
				currentAttackHitboxType = sprintAttackTwo.AttackHitbox;

				currentAttackDataToSend.damageAmount = sprintAttackTwo.damageValue;
				currentAttackDataToSend.hitstunStrength = sprintAttackTwo.hitstunValue;

				GetPlayerCharacterMovementComponent()->setRootMotionVelocityMultiplier(1.f * sprintAttackTwo.rootMotionMultiplier);
				GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);				
			}
		}
		//debugSpawnFX();
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

void APlayerCharacter::OnOverlapBeginAttackHit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
											   UPrimitiveComponent * OtherComp, int32 OtherBodyIndex,
											   bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) // Default nullptr and self check // (OtherComp->IsA(USkeletalMesh::StaticClass()))
	{
		
		ICharacterInterface* characterInterface = Cast<ICharacterInterface>(OtherActor);
		//UE_LOG(LogTemp, Warning, TEXT("Has interface: %s"), (characterInterface ? TEXT("True") : TEXT("False")));
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
				if (hitDirection.Size() < 1.f) // If getting physics velocity fails
				{
					hitDirection = GetMesh()->GetRightVector().GetSafeNormal(0.000001f);
				}

				if ((currentActionType == EActionType::DefaultComboOne || currentActionType == EActionType::DefaultComboTwo))
				{
					FAttackData tempAttackData; // TODO: Refactor
					tempAttackData = calculateChargeAttackValues(currentChargeAttackDataToSend);

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

				debugSpawnHitFX(OverlappedComp->GetComponentLocation());
				characterInterface->Execute_takeDamage(OtherActor, currentAttackDataToSend);
			}								
		}
	}
}

FAttackData APlayerCharacter::getCurrentAttackData()
{
	FAttackData currentAttackData;
	currentAttackData = calculateChargeAttackValues(currentChargeAttackDataToSend);

	currentAttackData = FAttackData(currentAttackData.damageAmount,
	FVector::ZeroVector, FVector::ZeroVector,
	this, currentAttackData.hitstunStrength);

	return currentAttackData;
}

void APlayerCharacter::takeDamage_Implementation(FAttackData inAttackData)
{
	//Super::takeDamage_Implementation(inAttackData);

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
	//UE_LOG(LogTemp, Warning, TEXT("Take damage player char: %s"), *this->GetName());

}

void APlayerCharacter::hitstunReset()
{
	resetAttackCombos();
	clearHitActors(); // TODO(?): May not be necessary

	PlayerCharacterMovementComponent->bIgnoreVerticalHit = false;

	bDodgingActive = false;
	bChargeAttackStarted = false;
	bCanSprintAttack = false;
	bAttackHitboxActive = false;
	bAttackActionActive = false;
	bStandbyActive = true;
	bDefaultAttackStarted = false;
	bCanCancelAction = false;

	//bChargeAttackInputHeld = false;
	//bMinimumChargeReached = false;

	debugDespawnFX();
	updateMovement();
	Execute_deactivateAttackHitbox(this);
	
	updateMovement();

	if (currentDefaultAttackData.AttackHitbox == EAttackHitboxType::Default && Weapon->GetChildActor())
	{
		Execute_deactivateAttackHitbox(Weapon->GetChildActor());
	}
}

void APlayerCharacter::runHitstunProcedure(float inHitstunStrengthReceived, FVector hitDirection)
{
	//UE_LOG(LogTemp, Warning, TEXT("Start hitstun procedure"))
	if (inHitstunStrengthReceived <= 0.1f || bIgnoreHitstun)
	{
		if (IsValid(hitstunAnimations.hitstunLightAnimMontage))
		{
			//currentMontage = hitstunAnimations.hitstunLightAnimMontage;
			//GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		}
	}
	else if (inHitstunStrengthReceived > 0.1f && inHitstunStrengthReceived <= 0.3f)
	{
		if (IsValid(hitstunAnimations.hitstunLightAnimMontage) && !bIsLaunched)
		{
			currentMontage = hitstunAnimations.hitstunHeavyAnimMontage;
			GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
			hitstunReset();
		}
		// TODO: Make stun timer
	}
	else if (inHitstunStrengthReceived > 0.3f && inHitstunStrengthReceived <= 0.7f && !bIsLaunched)
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

		GetMesh()->GetAnimInstance()->Montage_Stop(0.15f, currentMontage);

		FVector tempDirection = currentReceivedAttackData.hitDirection * -1; // Sets rotation to follow direction
		SetActorRotation(FRotator(0.f, tempDirection.ToOrientationRotator().Yaw, 0.f));

		FVector adjustedDirection = (FVector(hitDirection.X, hitDirection.Y, 0).GetSafeNormal()) * calculateLaunchLength(inHitstunStrengthReceived) + FVector(0.f, 0.f, tempLaunchZaxis);
		LaunchCharacter(adjustedDirection, false, false);

		startLaunch();
		hitstunReset();
	}
	else
	{
		// Debug else, function should normally not reach this line // Update: Can reach this part if player is hit when launched and in air
		UE_LOG(LogTemp, Warning, TEXT("You dun goofed, or player was hit when launched"))
	}
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

float APlayerCharacter::getWindUpChargeAmount_Implementation()
{
	return tickWindUpChargeAmount;
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

void APlayerCharacter::startDodge_Implementation()
{
	bDodgingActive = true;
}

void APlayerCharacter::endDodge_Implementation()
{
	bDodgingActive = false;
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
	case EAttackHitboxType::LeftKnee:
		overlapCollisionToEnable = LeftKneeHitbox;
		break;
	case EAttackHitboxType::RightKnee:
		overlapCollisionToEnable = RightKneeHitbox;
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
	LeftKneeHitbox->SetGenerateOverlapEvents(false);
	RightKneeHitbox->SetGenerateOverlapEvents(false);

	LeftHandHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftFootHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightFootHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftKneeHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightKneeHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void APlayerCharacter::interact()
{
	if (bInteractableObjectInRange && canAttack())
	{
		if (currentInteractableObject != nullptr) // TODO(?): May not be necessary
		{
			Execute_interact(currentInteractableObject, this);
		}
	}
	else
	{
		actionPressed(InteractionMainActionType);
	}
}

void APlayerCharacter::setInteractableObjectInRange_Implementation(AInteractableObject * inObject)
{
	currentInteractableObject = inObject;
	bInteractableObjectInRange = true;
}

bool APlayerCharacter::getInteractableObjectInRange_Implementation()
{
	return bInteractableObjectInRange;
}

void APlayerCharacter::startMovesetChange(bool nextMoveset)
{
	if (canAttack() && (unlockedCombatMovesets.Num() > 1))
	{
		bChangeNextMoveset = nextMoveset;

		if (movesetChangeMontage != nullptr)
		{
			currentMontage = movesetChangeMontage;
			GetMesh()->GetAnimInstance()->Montage_Play(currentMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);

			Execute_setWeaponVisibility(Weapon->GetChildActor(), false);

			eventStartChangeMoveset(bChangeNextMoveset);
		}
		else
		{
			Execute_endMovesetChange(this);
		}
	}

}

void APlayerCharacter::endMovesetChange_Implementation()
{
	if (bChangeNextMoveset)
	{
		findNextMoveset(true);
	}
	else
	{
		findPreviousMoveset(true);
	}

	Execute_setWeaponVisibility(Weapon->GetChildActor(), true);
}
