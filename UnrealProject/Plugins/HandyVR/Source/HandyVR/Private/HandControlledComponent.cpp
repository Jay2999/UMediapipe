// Fill out your copyright notice in the Description page of Project Settings.

#include "HandControlledComponent.h"
#include "HandTrackerComponent.h"

UHandControlledComponent::UHandControlledComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UHandControlledComponent::BeginPlay()
{
	Super::BeginPlay();
	auto actor = GetOwner();
	if (actor) {
		auto _tracker = actor->FindComponentByClass<UHandTrackerComponent>();
		if (_tracker) {
			this->tracker = _tracker;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Could not find HandTrackerComponent in this actor."));
		}
	}
}

void UHandControlledComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (tracker) {
		HandData handData = laterality == Handedness::LEFT ? tracker->pollHandDataLeft() : tracker->pollHandDataRight();
		SetRelativeTransform(handData.transform);
	}
}