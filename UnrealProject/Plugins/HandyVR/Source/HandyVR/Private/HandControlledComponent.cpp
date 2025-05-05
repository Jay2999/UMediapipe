// Fill out your copyright notice in the Description page of Project Settings.

#include "HandControlledComponent.h"

UHandControlledComponent::UHandControlledComponent() : writeIndex(0)
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

	int wIndex = writeIndex.load();
	int firstIndex = wIndex;
	int numElems = 4;
	for (int i = 0; i < numElems; ++i) firstIndex = handData.GetPreviousIndex(firstIndex);
	FVector loc = handData[firstIndex].transform.GetLocation();
	FQuat rot = handData[firstIndex].transform.GetRotation();

	float ratio = 1.0f / numElems;
	for (int i = handData.GetNextIndex(firstIndex); i != wIndex; i = handData.GetNextIndex(i)) {
		loc = FMath::Lerp<FVector, float>(loc, handData[i].transform.GetLocation(), ratio);
		rot = FQuat::Slerp(rot, handData[i].transform.GetRotation(), ratio);
	}
	SetRelativeTransform(FTransform(rot, loc, FVector(1, 1, 1)));

	int prev0 = handData.GetPreviousIndex(wIndex);
	int prev1 = handData.GetPreviousIndex(prev0);
	if (handData[prev1].gesture != handData[prev0].gesture)
	{
		OnHandGestureEvent.Broadcast(handData[prev0].gesture);
	}
}

void UHandControlledComponent::pollFingerAngles(TArray<FVector>& angles) const
{
	int wIndex = writeIndex.load();
	int firstIndex = wIndex;
	int numElems = 3;
	for (int i = 0; i < numElems; ++i) firstIndex = handData.GetPreviousIndex(firstIndex);
	if (angles.Num() != 4) {
		angles = TArray<FVector>(handData[firstIndex].fingersAngles, 4);
	}
	else {
		memcpy(angles.GetData(), handData[firstIndex].fingersAngles, 4 * sizeof(FVector));
	}
	float ratio = 1.0f / numElems;
	for (int i = handData.GetNextIndex(firstIndex); i != wIndex; i = handData.GetNextIndex(i)) {
		for (int j = 0; j < 4; ++j) {
			angles[j] = FMath::Lerp<FVector, float>(angles[j], handData[i].fingersAngles[j], ratio);
		}
	}
}

void UHandControlledComponent::pollThumbAngles(float& pitch, FVector& zAngles) const
{
	int wIndex = writeIndex.load();
	int firstIndex = wIndex;
	int numElems = 3;
	for (int i = 0; i < numElems; ++i) firstIndex = handData.GetPreviousIndex(firstIndex);
	pitch = handData[firstIndex].thumbPitch;
	zAngles = handData[firstIndex].thumbAngles;
	float ratio = 1.0f / numElems;
	for (int i = handData.GetNextIndex(firstIndex); i != wIndex; i = handData.GetNextIndex(i)) {
		pitch = (1.0f - ratio) * pitch + ratio * handData[i].thumbPitch;
		zAngles = FMath::Lerp<FVector, float>(zAngles, handData[i].thumbAngles, ratio);
	}
}