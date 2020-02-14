// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InterfaceAI.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterfaceAI : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DAWNOFSOLARIS_API IInterfaceAI
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UPROPERTY(EditAnywhere, Category = "Behavior")
		class UBehaviorTree* getBehaviorTree();

};
