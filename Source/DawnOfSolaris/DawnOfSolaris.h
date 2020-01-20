// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"

#include "DawnOfSolaris.generated.h"

//UE_LOG(LogTemp, Warning, TEXT("Reference UE_LOG"))

UENUM(BlueprintType)
enum EAttackHitboxType
{
	LeftHand     UMETA(DisplayName = "Left Hand"),
	LeftElbow    UMETA(DisplayName = "Left Elbow"),
	LeftKnee	 UMETA(DisplayName = "Left Knee"),
	LeftFoot     UMETA(DisplayName = "Left Hand"),

	RightHand     UMETA(DisplayName = "Right Hand"),
	RightElbow    UMETA(DisplayName = "Right Elbow"),
	RightKnee     UMETA(DisplayName = "Right Knee"),
	RightFoot     UMETA(DisplayName = "Right Hand")
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

};

UENUM()
enum EAttackType
{
	DefaultAttack     UMETA(DisplayName = "Default Attack"),
	AttackOneCombo     UMETA(DisplayName = "Attack One Combo"),
	AttackTwoCombo    UMETA(DisplayName = "Attack Two Combo"),
	GrabAttack    UMETA(DisplayName = "Grab Attack"),
};