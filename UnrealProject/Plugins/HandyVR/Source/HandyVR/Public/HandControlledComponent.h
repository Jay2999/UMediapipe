// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HandData.hpp"
#include "HandControlledComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHandGestureEvent, HandGestures, Gesture);

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

	TArray<HandData> handData;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<Handedness> laterality = Handedness::LEFT;

	UPROPERTY(BlueprintAssignable)
	FHandGestureEvent OnHandGestureEvent;

	inline void pushHandData(HandData&& data) {
		handData.Add(MoveTemp(data));
		if (handData.Num() > 5) handData.RemoveAt(0);
	}

	UFUNCTION(BlueprintCallable)
	TArray<FVector> pollFingertips() const;
};
