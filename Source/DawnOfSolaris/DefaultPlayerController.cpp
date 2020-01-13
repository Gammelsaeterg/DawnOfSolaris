// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerController.h"
#include "Blueprint/UserWidget.h"


void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Create the widget and store it.
	PlayerUI = CreateWidget<UUserWidget>(this, wPlayerUI);
	PlayerUI->AddToViewport();
}

void ADefaultPlayerController::PauseGame()
{
	if (wPauseMenu) // Check if the Asset is assigned in the blueprint.
	{
		// Create the widget and store it.
		PauseMenu = CreateWidget<UUserWidget>(this, wPauseMenu);

		//// Extra check to  make sure the pointer holds the widget.
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();

			// TODO: May not need this code
			PauseMenu->bIsFocusable = true;

			//Show the Cursor.
			bShowMouseCursor = true;

			Pause();
		}


	}
}

void ADefaultPlayerController::ContinueGame()
{
	SetPause(false);

	if (PauseMenu)
	{
		PauseMenu->RemoveFromParent();
		bShowMouseCursor = false;

		SetInputMode(FInputModeGameOnly());
	}
}
