// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HandEnumLibrary.h"
#include "HandControlledComponent.generated.h"

class UHandTrackerComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (HandyVR), meta = (BlueprintSpawnableComponent))
class HANDYVR_API UHandControlledComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHandControlledComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	UHandTrackerComponent* tracker;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<Handedness> laterality = Handedness::LEFT;


};
