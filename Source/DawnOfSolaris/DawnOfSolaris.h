// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"

#include "DawnOfSolaris.generated.h"

//UE_LOG(LogTemp, Warning, TEXT("Reference UE_LOG"))

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

USTRUCT(BlueprintType)
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
struct FHitstunData
{
	//GENERATED_USTRUCT_BODY()
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HitstunAnimMontage;

	// TODO(?): Add more variables
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

UENUM()
enum EAttackType
{
	DefaultAttack     UMETA(DisplayName = "Default Attack"),
	AttackOneCombo    UMETA(DisplayName = "Attack One Combo"),
	AttackTwoCombo    UMETA(DisplayName = "Attack Two Combo"),
	GrabAttack		  UMETA(DisplayName = "Grab Attack")
};

UENUM()
enum EActionInputs
{
	Action0      UMETA(DisplayName = "Action0 (Interact)"),
	Action1      UMETA(DisplayName = "Action1 (Attack 0)"),
	Action2      UMETA(DisplayName = "Action2 (Attack 1)"),
	Action3		 UMETA(DisplayName = "Action3 (Attack 2 / Grab attack)")
};

UENUM(BlueprintType)
enum class ECombatAlignment : uint8
{
	Neutral    UMETA(DisplayName = "Neutral"), // Neutrals can damage interact with anyone, also default for destructible barrier and other props
	Human      UMETA(DisplayName = "Human"), // Humans can damage interact with Aliens, Players and Neutrals
	Alien      UMETA(DisplayName = "Alien"), // Aliens can damage interact with Humans, Neutrals
	Player     UMETA(DisplayName = "Player") // Players can damage interact with Humans, Neutrals and other Players
};

bool canDamageInteract(ECombatAlignment selfAlignment, ECombatAlignment otherAlignment);

USTRUCT(BlueprintType) //// TODO(?): Utilize later
struct FMovementData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	float maxWalkSpeed{ 600.f };

	UPROPERTY(BlueprintReadOnly)
	float maxRotationRate{ 540.f };

	FMovementData(float inMaxWalkSpeed, float inMaxRotationRate) : maxWalkSpeed(inMaxWalkSpeed), maxRotationRate(inMaxRotationRate) {}
	FMovementData() : maxWalkSpeed(600.f), maxRotationRate(540.f) {}
};


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