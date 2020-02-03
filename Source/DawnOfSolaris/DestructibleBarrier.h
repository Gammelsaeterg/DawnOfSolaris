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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterVariables")
	ECombatAlignment CombatAlignment = ECombatAlignment::Neutral;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDestroyed{ false };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterCombat")
	void takeDamage(FAttackData inAttackData);
	virtual void takeDamage_Implementation(FAttackData inAttackData) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterVariables")
	ECombatAlignment getAlignment();
	virtual ECombatAlignment getAlignment_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
	void breakBarrierBlueprintEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void takeDamageBlueprintEvent(FVector hitDirection);
};
