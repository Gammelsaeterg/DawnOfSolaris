// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"
#include "Engine/Texture2D.h"
#include "Animation/AnimBlueprint.h"

#include "DawnOfSolaris.generated.h"

//UE_LOG(LogTemp, Warning, TEXT("Reference UE_LOG"))

// Global hitstun variables
const static float flinchOnlyMinLimit{ 0.1f };
const static float flinchAndKnockbackMinLimit{ 0.3f };
const static float launchMinLimit{ 0.7f };

UENUM(BlueprintType)
enum class EAttackHitboxType : uint8
{
	Default		  UMETA(DisplayName = "Default"),
	HeldObject    UMETA(DisplayName = "Held object"),
	Projectile    UMETA(DisplayName = "Projectile"),

	LeftHand      UMETA(DisplayName = "Left Hand"),
	LeftElbow     UMETA(DisplayName = "Left Elbow"),
	LeftKnee	  UMETA(DisplayName = "Left Knee"),
	LeftFoot      UMETA(DisplayName = "Left Foot"),

	RightHand     UMETA(DisplayName = "Right Hand"),
	RightElbow    UMETA(DisplayName = "Right Elbow"),
	RightKnee     UMETA(DisplayName = "Right Knee"),
	RightFoot     UMETA(DisplayName = "Right Foot"),
};

USTRUCT(BlueprintType) // TODO: Add ChargeTime (default is 1.f (1 second)), add RootAnim multiplier
struct FChargeAttackData
{
	//GENERATED_USTRUCT_BODY()
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minDamageValue{ 30 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxDamageValue{ 60 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minHitstunValue{ 0.3f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxHitstunValue{ 0.6f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float rootMotionMultiplier{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxChargeTime{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float baseStaminaConsumptionRate{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float projectileScaleMultiplier{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackHitboxType AttackHitbox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUnstoppable{ false };
};

USTRUCT(BlueprintType)
struct FSprintAttackData
{
	//GENERATED_USTRUCT_BODY()
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* sprintAttackAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float damageValue{ 20 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float hitstunValue{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float staminaCost{ 15.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float rootMotionMultiplier{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackHitboxType AttackHitbox;
};

USTRUCT(BlueprintType)
struct FDefaultAttackData
{
	//GENERATED_USTRUCT_BODY()
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float damageValue{ 20 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float hitstunValue{ 0.1f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackHitboxType AttackHitbox;
};

USTRUCT(BlueprintType)
struct FAttackData // Used for storing and sending attack info
{
	//GENERATED_USTRUCT_BODY()
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float damageAmount{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector hitDirection{0.f, 0.f, 0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector hitLocation{0.f, 0.f, 0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* damageDealingActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float hitstunStrength{ 0 };

	FAttackData(float inDamageAmount, FVector inHitDirection, FVector inHitLocation, AActor* inDamageDealingActor, float inHitstunStrength)
	: damageAmount(inDamageAmount), hitDirection(inHitDirection), 
	  hitLocation(inHitLocation), damageDealingActor(inDamageDealingActor), hitstunStrength(inHitstunStrength) {}

	FAttackData()
	: damageAmount(0.f), hitDirection(FVector{ 0.f, 0.f, 0.f }),
	hitLocation(FVector{ 0.f, 0.f, 0.f }), damageDealingActor(nullptr), hitstunStrength(0.f) {}
};

USTRUCT(BlueprintType)
struct FDodgeRollData // Used for storing and sending attack info
{
	//GENERATED_USTRUCT_BODY()
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* dodgeAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float dodgeRootAnimationMultiplier{ 1.f };
};

UENUM()
enum class EActionType : uint8 // TODO: Add more actions
{
	DefaultAttack = 2       UMETA(DisplayName = "Default Attack"),
	DefaultComboOne = 0     UMETA(DisplayName = "Attack One Combo"),
	DefaultComboTwo	= 1		UMETA(DisplayName = "Attack Two Combo"),
	GrabAttack				UMETA(DisplayName = "Grab Attack"),
	Interact				UMETA(DisplayName = "Interact"),
	Sprint					UMETA(DisplayName = "Sprint"),
	SprintAttackOne			UMETA(DisplayName = "SprintAttackOne"),
	SprintAttackTwo			UMETA(DisplayName = "SprintAttackTwo"),
	DodgeRoll			    UMETA(DisplayName = "DodgeRoll"),
	NONE					UMETA(DisplayName = "NONE")
};

UENUM()
enum class EActionInput : uint8
{
	Action0      UMETA(DisplayName = "Action0 (Interact)"),
	Action1      UMETA(DisplayName = "Action1 (Attack 1)"),
	Action2      UMETA(DisplayName = "Action2 (Attack 2)"),
	Action3		 UMETA(DisplayName = "Action3 (Attack 3 / Grab attack)")
};

UENUM()
enum class EHitstunType : uint8
{
	hitstunAnimationOnly			UMETA(DisplayName = "Hitstun (animation only)"),
	hitstunFlinch					UMETA(DisplayName = "Hitstun (stunned in place)"),
	hitstunFlinchWithKnockback      UMETA(DisplayName = "Hitstun (stunned and knockback)"),
	hitstunLaunched					UMETA(DisplayName = "Hitstun (launched in air)")
};

USTRUCT(BlueprintType)
struct FHitstunData
{
	//GENERATED_USTRUCT_BODY()
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* hitstunLightAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* hitstunHeavyAnimMontage;

	// TODO(?): Add more variables
};

USTRUCT(BlueprintType)
struct FMovesetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString movesetName { "Set moveset name here" };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* movesetIcon;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class ABaseWeapon* playerWeaponMorph{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class TSubclassOf<class ABaseWeapon> playerWeaponMorph;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> ChargeAttacksOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChargeAttackData> ChargeAttacksTwo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSprintAttackData SprintAttackOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSprintAttackData SprintAttackTwo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimBlueprint* movesetAnimBlueprint;
};

//UENUM() // TODO(?): Complete or delete
//enum class EHitstunGrade : uint8
//{
//	hitstunGrade1			UMETA(DisplayName = "hitstunGrade1"),
//	hitstunGrade2			UMETA(DisplayName = "hitstunGrade2"),
//	hitstunGrade3			UMETA(DisplayName = "hitstunGrade3"),
//	hitstunGrade4			UMETA(DisplayName = "hitstunGrade4")
//};


UENUM(BlueprintType)
enum class ECombatAlignment : uint8
{
	Neutral    UMETA(DisplayName = "Neutral"), // Neutrals can damage interact with anyone, also default for destructible barrier and other props
	Human      UMETA(DisplayName = "Human"), // Humans can damage interact with Aliens, Players and Neutrals
	Alien      UMETA(DisplayName = "Alien"), // Aliens can damage interact with Humans, Neutrals
	Player     UMETA(DisplayName = "Player") // Players can damage interact with Humans, Neutrals and other Players
};

bool canDamageInteract(ECombatAlignment selfAlignment, ECombatAlignment inAlignment);

USTRUCT(BlueprintType) //// TODO(?): Utilize later
struct FMovementData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	float maxWalkSpeed{ 600.f };

	UPROPERTY(BlueprintReadOnly)
	float maxRotationRate{ 540.f };

	UPROPERTY(BlueprintReadOnly)
	float rotInterpSpeed{ 10.f };

	UPROPERTY(BlueprintReadOnly)
	float walkSpeedInterpSpeed{ 10.f };


	FMovementData(float inMaxWalkSpeed, float inMaxRotationRate) : maxWalkSpeed(inMaxWalkSpeed), maxRotationRate(inMaxRotationRate) {}
	FMovementData(float inMaxWalkSpeed, float inMaxRotationRate, float inRotInterpSpeed, float inWalkSpeedInterpSpeed) 
	: maxWalkSpeed(inMaxWalkSpeed), maxRotationRate(inMaxRotationRate), rotInterpSpeed(inRotInterpSpeed), walkSpeedInterpSpeed(inWalkSpeedInterpSpeed) {}
	FMovementData() : maxWalkSpeed(600.f), maxRotationRate(540.f) {}
};

float calculateKnockbackLength(float inMultiplier);
float calculateLaunchLength(float inMultiplier);

template<typename TEnum>
static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}
	return enumPtr->GetNameByValue((int64)Value).ToString();
}