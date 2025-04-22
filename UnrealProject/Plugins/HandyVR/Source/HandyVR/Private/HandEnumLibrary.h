// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HandEnumLibrary.generated.h"

UENUM(BlueprintType)
enum Handedness
{
	LEFT,
	RIGHT
};

UENUM(BlueprintType)
enum HandGestures
{
	NONE,
	OPEN_PALM,
	CLOSED_FIST
};