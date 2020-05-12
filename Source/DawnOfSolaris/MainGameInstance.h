// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DAWNOFSOLARIS_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float soundVolume{ 100.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float musicVolume{ 100.f };
};
