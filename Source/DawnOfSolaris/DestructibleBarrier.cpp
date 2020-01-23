// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleBarrier.h"
#include "Engine/StaticMesh.h"

// Sets default values
ADestructibleBarrier::ADestructibleBarrier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	baseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
}

// Called when the game starts or when spawned
void ADestructibleBarrier::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADestructibleBarrier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestructibleBarrier::takeDamage_Implementation(float damageAmount, FVector hitDirection, FVector hitLocation, AActor* damageDealingActor, float hitstunStrength)
{
	if (currentHealthPoints > damageAmount)
	{
		currentHealthPoints -= damageAmount;
		UE_LOG(LogTemp, Warning, TEXT("Took damage: %f, health left: %f"), damageAmount, currentHealthPoints);
	}
	else
	{
		currentHealthPoints = 0;
		breakBarrier();
	}
}

