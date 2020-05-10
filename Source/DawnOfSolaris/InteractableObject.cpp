// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableObject.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
AInteractableObject::AInteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TriggerLocation"));
	TriggerLocation->SetupAttachment(RootComponent);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AInteractableObject::OnOverlapBeginTriggerBox);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AInteractableObject::OnOverlapEndTriggerBox);  // TODO(?): May need this later
}

// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	
	//if (!bAutoTrigger)
	//{
	//	TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &AInteractableObject::OnOverlapBeginTriggerBox);
	//}
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableObject::OnOverlapBeginTriggerBox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
												   UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, 
	                                               bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		ICharacterInterface* characterInterface = Cast<ICharacterInterface>(OtherActor);
		if (characterInterface)
		{
			if (alignmentThatCanTrigger == Execute_getAlignment(OtherActor) && !actorIsAlreadyOverlapping(OverlappedActorAndComponent(OtherActor, OtherComp)))
			{
				if (!bAutoTrigger && !bHasDoneOnce)
				{
					Execute_setInteractableObjectInRange(OtherActor, this);
					actorsOverlappingTriggerBox.Add(OverlappedActorAndComponent(OtherActor, OtherComp));

					//objectInteracted(OtherActor); // For debugging
				}
				else
				{
					if (bDoOnce)
					{
						if (!bHasDoneOnce)
						{
							objectInteracted(OtherActor);
							actorsOverlappingTriggerBox.Add(OverlappedActorAndComponent(OtherActor, OtherComp));

							bHasDoneOnce = true;
						}

					}
					else
					{
						objectInteracted(OtherActor);
						actorsOverlappingTriggerBox.Add(OverlappedActorAndComponent(OtherActor, OtherComp));
					}
				}		
			}
		}
	}
}


FVector AInteractableObject::interact_Implementation(AActor* interactorActor)
{
	if (bDoOnce)
	{
		if (!bHasDoneOnce)
		{
			bHasDoneOnce = true;
			objectInteracted(interactorActor);
			TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			return TriggerLocation->GetComponentLocation();
		}
		else
		{
			return TriggerLocation->GetComponentLocation();
		}
	}
	else
	{
		objectInteracted(interactorActor);
		return TriggerLocation->GetComponentLocation();
	}
}

void AInteractableObject::OnOverlapEndTriggerBox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,  // TODO(?): May need this later
	                                             UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		ICharacterInterface* characterInterface = Cast<ICharacterInterface>(OtherActor);
		if (characterInterface)
		{
			removeCheck(OverlappedActorAndComponent(OtherActor, OtherComp));
		}
	}
}

bool AInteractableObject::actorIsAlreadyOverlapping(OverlappedActorAndComponent inOverlappedActorAndComponent)
{
	bool overlappedActorAndComponentFound{ false };
	for (auto actorAndComponent : actorsOverlappingTriggerBox)
	{
		if (actorAndComponent.OverlappedActor == inOverlappedActorAndComponent.OverlappedActor)
		{
			overlappedActorAndComponentFound = true;
		}
	}

	return overlappedActorAndComponentFound;
}

void AInteractableObject::removeCheck(OverlappedActorAndComponent inOverlappedActorAndComponent)
{
	for (int i = 0; i < actorsOverlappingTriggerBox.Num(); ++i)
	{
		if (actorsOverlappingTriggerBox[i].OverlappedActor == inOverlappedActorAndComponent.OverlappedActor
			&& actorsOverlappingTriggerBox[i].OverlappedComponent == inOverlappedActorAndComponent.OverlappedComponent)
		{
			actorsOverlappingTriggerBox.RemoveAt(i, 1, true);
		}
	}
	
}
