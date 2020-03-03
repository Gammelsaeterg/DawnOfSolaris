// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterInterface.h"

#include "InteractableObject.generated.h"

UCLASS()
class DAWNOFSOLARIS_API AInteractableObject : public AActor, public ICharacterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableObject();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractionObjectSettings")
	bool bAutoTrigger{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractionObjectSettings")
	bool bDoOnce{ false };

	bool bHasDoneOnce{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractionObjectSettings")
	ECombatAlignment alignmentThatCanTrigger{ ECombatAlignment::Player };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractionObjectSettings")
	bool faceCharacterTowardsTriggerLocation{ true };

	UPROPERTY(EditAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* TriggerLocation;

	UFUNCTION()
	void OnOverlapBeginTriggerBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								  bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION() // TODO(?): May need this later
	void OnOverlapEndTriggerBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void objectInteracted(class AActor* InteractorActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void interact(class AActor* InteractorActor);
	virtual void interact_Implementation(class AActor* interactorActor) override;


	// TODO(?): Code below may be overcomplicated, investigate this later
	struct OverlappedActorAndComponent
	{
		AActor* OverlappedActor;
		UPrimitiveComponent* OverlappedComponent;

		OverlappedActorAndComponent(AActor* inOverlappedActor, UPrimitiveComponent* inOverlappedComponent)
		: OverlappedActor(inOverlappedActor), OverlappedComponent(inOverlappedComponent) {}
	};
	TArray<OverlappedActorAndComponent> actorsOverlappingTriggerBox;
	bool actorIsAlreadyOverlapping(OverlappedActorAndComponent inOverlappedActorAndComponent);
	void removeCheck(OverlappedActorAndComponent inOverlappedActorAndComponent);
};
