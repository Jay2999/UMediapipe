// Fill out your copyright notice in the Description page of Project Settings.


#include "HandTrackerComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "MediaPlayer.h"
#include "HandData.hpp"
#include "HandControlledComponent.h"

static TArray<UHandControlledComponent*> leftControlledCtx;
static TArray<UHandControlledComponent*> rightControlledCtx;

static void handCallback(ump::HandDetectionResult* hand) {
	auto handData = toHandData(*hand);
	if (handData.handedness == Handedness::LEFT) {
		for (auto& comps : leftControlledCtx) {
			comps->pushHandData(MoveTemp(handData));
		}
	}
	else {
		for (auto& comps : rightControlledCtx) {
			comps->pushHandData(MoveTemp(handData));
		}
	}
}

UHandTrackerComponent::UHandTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/HandyVR/Camera/CameraTextureMaterial.CameraTextureMaterial"));
	if (MaterialFinder.Succeeded()) {
		cameraTextureMaterial = MaterialFinder.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Could not find CameraTextureMaterial."));
	}
	ConstructorHelpers::FObjectFinder<UMediaTexture> TextureFinder(TEXT("/HandyVR/Camera/DefaultMediaTexture.DefaultMediaTexture"));
	if (TextureFinder.Succeeded()) {
		mediaTexture = TextureFinder.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Could not find DefaultMediaTexture."));
	}
}

UHandTrackerComponent::~UHandTrackerComponent()
{
	if (_ump) _ump->stopHandDetection();
	_ump.Release();
}

// Called when the game starts
void UHandTrackerComponent::BeginPlay()
{
	Super::BeginPlay();
	mediaTexture->GetMediaPlayer()->OnPlaybackResumed.AddDynamic(this, &UHandTrackerComponent::OnPlayingVideo);

	auto actor = GetOwner();
	if (actor) {
		TInlineComponentArray<UHandControlledComponent*> controlledComponents(actor, true);
		for (const auto& comp : controlledComponents) {
			if (comp->laterality == Handedness::LEFT) {
				leftControlledCtx.Add(comp);
			}
			else {
				rightControlledCtx.Add(comp);
			}
		}
	}
}

void UHandTrackerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	auto actor = GetOwner();
	if (actor) {
		TInlineComponentArray<UHandControlledComponent*> controlledComponents(actor, true);
		for (const auto& comp : controlledComponents) {
			if (comp->laterality == Handedness::LEFT) {
				leftControlledCtx.Remove(comp);
			}
			else {
				rightControlledCtx.Remove(comp);
			}
		}
	}
}

void UHandTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	processCameraFrame();
}

void UHandTrackerComponent::OnPlayingVideo()
{
	auto dims = mediaTexture->GetMediaPlayer()->GetVideoTrackDimensions(INDEX_NONE, INDEX_NONE);
	if (targetCameraWidth < 0) {
		targetCameraWidth = dims.X;
	}
	if (targetCameraHeight < 0) {
		targetCameraHeight = dims.Y;
	}
	cameraTextureRT = UKismetRenderingLibrary::CreateRenderTarget2D(this, targetCameraWidth, targetCameraHeight, RTF_RGBA8);
	pixels.AddUninitialized(targetCameraHeight * targetCameraWidth);
	_ump = MakeUnique<ump::UMediapipe>(handCallback, targetCameraWidth, targetCameraHeight);
	_ump->beginHandDetection();
}

void UHandTrackerComponent::processCameraFrame()
{
	if (!cameraTextureRT) return;
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, cameraTextureRT, cameraTextureMaterial);

	auto* rt = cameraTextureRT->GameThread_GetRenderTargetResource();
	if (rt && rt->ReadPixels(pixels)) {
		_ump->sendFrame((uint8_t*)pixels.GetData());
	}
}
