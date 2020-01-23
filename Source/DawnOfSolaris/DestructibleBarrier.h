// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterInterface.h"
#include "DestructibleBarrier.generated.h"

UCLASS()
class DAWNOFSOLARIS_API ADestructibleBarrier : public AActor, public ICharacterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleBarrier();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Base mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* baseMesh;

};
