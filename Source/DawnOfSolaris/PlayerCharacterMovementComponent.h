// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class DAWNOFSOLARIS_API UPlayerCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;

	virtual void PerformMovement(float DeltaTime) override;

	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;
	bool bHitVerticalObject{ false };
	float angleThreshold{ 50.f };
};
