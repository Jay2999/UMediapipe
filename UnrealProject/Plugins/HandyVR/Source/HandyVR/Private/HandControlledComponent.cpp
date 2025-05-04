// Fill out your copyright notice in the Description page of Project Settings.

#include "HandControlledComponent.h"

UHandControlledComponent::UHandControlledComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UHandControlledComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHandControlledComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	int firstIndex = writeIndex;
	for (int i = 0; i < 3; ++i) firstIndex = handData.GetNextIndex(firstIndex);
	FVector loc = handData[firstIndex].transform.GetLocation();
	FQuat rot = handData[firstIndex].transform.GetRotation();

	int numElems = 4;
	float ratio = 1.0f / numElems;
	for (int i = handData.GetNextIndex(firstIndex); handData.GetNextIndex(i) != writeIndex; i = handData.GetNextIndex(i)) {
		loc = FMath::Lerp<FVector, float>(loc, handData[i].transform.GetLocation(), ratio);
		rot = FQuat::Slerp(rot, handData[i].transform.GetRotation(), ratio);
	}
	SetRelativeTransform(FTransform(rot, loc, FVector(1, 1, 1)));

	int prev0 = handData.GetPreviousIndex(writeIndex);
	int prev1 = handData.GetPreviousIndex(prev0);
	if (handData[prev1].gesture != handData[prev0].gesture)
	{
		OnHandGestureEvent.Broadcast(handData[prev0].gesture);
	}
}

void UHandControlledComponent::pollFingerAngles(TArray<FVector>& angles) const
{
	int firstIndex = writeIndex;
	for (int i = 0; i < 3; ++i) firstIndex = handData.GetNextIndex(firstIndex);
	if (angles.Num() != 4) {
		angles = TArray<FVector>(handData[firstIndex].fingersAngles, 4);
	}
	else {
		memcpy(angles.GetData(), handData[firstIndex].fingersAngles, 4 * sizeof(FVector));
	}
	int numElems = 4;
	float ratio = 1.0f / numElems;
	for (int i = handData.GetNextIndex(firstIndex); i != writeIndex; i = handData.GetNextIndex(i)) {
		for (int j = 0; j < 4; ++j) {
			angles[j] = FMath::Lerp<FVector, float>(angles[j], handData[i].fingersAngles[j], ratio);
		}
	}
	//return angles;
}
