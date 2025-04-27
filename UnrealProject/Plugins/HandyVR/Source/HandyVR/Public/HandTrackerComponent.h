// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "MediaTexture.h"
#include "UMediapipe.h"
#include "HandData.hpp"
#include "Containers/CircularQueue.h"
#include <functional>
#include "HandTrackerComponent.generated.h"

class UHandControlledComponent;

class CameraFrame {
	TArray<FColor> pixels;
	unsigned long long int hash;
public:
	CameraFrame() = default;
	CameraFrame(TArray<FColor>&& pixels);
	inline const TArray<FColor>& getPixels() const { return pixels; }
	inline bool operator==(const CameraFrame& other) const { return hash == other.hash; }
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (HandyVR), meta = (BlueprintSpawnableComponent))
class HANDYVR_API UHandTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHandTrackerComponent();
	~UHandTrackerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly)
	int targetCameraWidth = -1;
	
	UPROPERTY(EditDefaultsOnly)
	int targetCameraHeight = -1;

	UPROPERTY(EditDefaultsOnly)
	bool stripAlphaChannel = true;

private:
	UPROPERTY()
	UMediaTexture* mediaTexture = nullptr;

	UPROPERTY()
	UTextureRenderTarget2D* cameraTextureRT = nullptr;

	UPROPERTY()
	UMaterial* cameraTextureMaterial = nullptr;

	UFUNCTION()
	void OnPlayingVideo();

	TUniquePtr<TCircularQueue<CameraFrame>> frames = nullptr;

	uint8_t* rgbArray = nullptr;

	TUniquePtr<ump::UMediapipe> _ump = nullptr;

	void processCameraFrame();

	TArray<UHandControlledComponent*> leftHandControlledComponents;
	TArray<UHandControlledComponent*> rightHandControlledComponents;
};
