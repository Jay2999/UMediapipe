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

	if (handData.Num() == 0) return;
	FVector loc = handData[0].transform.GetLocation();
	FQuat rot = handData[0].transform.GetRotation();
	int numElems = handData.Num() - 2;
	float ratio = 1.0f / numElems;
	for (int i = 1; i < handData.Num() - 1; ++i) {
		loc = FMath::Lerp<FVector, float>(loc, handData[i].transform.GetLocation(), ratio);
		rot = FQuat::Slerp(rot, handData[i].transform.GetRotation(), ratio);
	}
	SetRelativeTransform(FTransform(rot, loc, FVector(1, 1, 1)));

	if (handData.Num() > 3 &&
		handData[0].gesture == handData[1].gesture &&
		handData[2].gesture == handData[3].gesture &&
		handData[1].gesture != handData[2].gesture)
	{
		OnHandGestureEvent.Broadcast(handData[2].gesture);
	}
}

TArray<FVector> UHandControlledComponent::pollFingertips() const
{
	TArray<FVector> angles;
	if (handData.Num() == 0) return angles;
	for (int i = 0; i < 4; ++i) {
		angles.Add(handData[0].fingersAngles[i]);
	}
	int numElems = handData.Num() - 2;
	float ratio = 1.0f / numElems;
	for (int i = 1; i < handData.Num() - 1; ++i) {
		for (int j = 0; j < 4; ++j) {
			angles[j] = FMath::Lerp<FVector, float>(angles[j], handData[i].fingersAngles[j], ratio);
		}
	}
	return angles;
}
