// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "DefaultPlayerController.generated.h"

// Forward declarations
class UUserWidget;

/**
 * 
 */
UCLASS()
class DAWNOFSOLARIS_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	// Override BeginPlay()
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void PauseGame();

	UFUNCTION(BlueprintCallable)
	void ContinueGame();

	// Pause menu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPauseMenu;

	// Player UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPlayerUI;

	// Variable to hold the widget After Creating it.
	UUserWidget* PauseMenu;

	UUserWidget* PlayerUI;
};
