// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "MediaTexture.h"
#include "UMediapipe.h"
#include "HandData.hpp"
#include "Containers/CircularQueue.h"
#include "HandTrackerComponent.generated.h"

class CameraFrame {
	TArray<FColor> pixels;
	unsigned long long int hash;
public:
	CameraFrame() = default;
	CameraFrame(TArray<FColor>&& pixels);
	inline const TArray<FColor>& getPixels() const { return pixels; }
	inline bool operator==(const CameraFrame& other) const { return hash == other.hash; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHandGestureEvent, HandGestures, Gesture);

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

	HandData pollHandDataLeft() const;

	HandData pollHandDataRight() const;

	UPROPERTY(BlueprintAssignable)
	FHandGestureEvent LeftHandGestureEvent;

	UPROPERTY(BlueprintAssignable)
	FHandGestureEvent RightHandGestureEvent;

private:
	UPROPERTY()
	UMediaTexture* mediaTexture = nullptr;

	UPROPERTY()
	UTextureRenderTarget2D* cameraTextureRT = nullptr;

	UPROPERTY()
	UMaterial* cameraTextureMaterial = nullptr;

	TUniquePtr<TCircularQueue<CameraFrame>> frames = nullptr;

	uint8_t* rgbArray = nullptr;

	TUniquePtr<ump::UMediapipe> _ump = nullptr;

	UFUNCTION()
	void OnPlayingVideo();

	void processCameraFrame();
};
