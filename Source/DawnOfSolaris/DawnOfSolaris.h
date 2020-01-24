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
	FVector hitFrom{ 0.f, 0.f, 0.f };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float hitstunStrength{ 0 };
};

UENUM()
enum EAttackType
{
	DefaultAttack     UMETA(DisplayName = "Default Attack"),
	AttackOneCombo    UMETA(DisplayName = "Attack One Combo"),
	AttackTwoCombo    UMETA(DisplayName = "Attack Two Combo"),
	GrabAttack		  UMETA(DisplayName = "Grab Attack")
};

UENUM(BlueprintType)
enum class ECombatAlignment : uint8
{
	Neutral    UMETA(DisplayName = "Neutral"), // Neutrals can damage interact with anyone
	Human      UMETA(DisplayName = "Human"), // Humans can damage interact with Aliens and Players
	Alien      UMETA(DisplayName = "Alien"), // Aliens can damage interact with Humans
	Player     UMETA(DisplayName = "Player") // Players can damage interact with Humans and other Players
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