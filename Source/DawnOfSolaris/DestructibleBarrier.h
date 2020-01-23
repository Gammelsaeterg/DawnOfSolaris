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

	// Root scene component
	class USceneComponent* rootComponent;

	// Base mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* baseMesh;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float maxHealthPoints{ 100 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	float currentHealthPoints{ 100 };

	bool bIsDestroyed{ false };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void takeDamage(float damageAmount, FVector hitDirection, FVector hitLocation, AActor* damageDealingActor, float hitstunStrength);
	virtual void takeDamage_Implementation(float damageAmount, FVector hitDirection, FVector hitLocation, AActor* damageDealingActor, float hitstunStrength) override;

	UFUNCTION(BlueprintImplementableEvent)
	void breakBarrier();
};
