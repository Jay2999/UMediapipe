// Fill out your copyright notice in the Description page of Project Settings.


#include "HandTrackerComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "MediaPlayer.h"
#include "HandData.hpp"

TArray<HandData> left;
TArray<HandData> right;

static void handCallback(ump::HandDetectionResult* hand) {
	auto handData = toHandData(*hand);
	if (handData.handedness == Handedness::LEFT) {
		left.Add(MoveTemp(handData));
		if (left.Num() > 5) left.RemoveAt(0);
	}
	else {
		right.Add(MoveTemp(handData));
		if (right.Num() > 5) right.RemoveAt(0);
	}
}

CameraFrame::CameraFrame(TArray<FColor>&& pixels) : pixels(std::move(pixels))
{
	hash = 0;
	for (int i = 0; i < this->pixels.Num(); i += this->pixels.Num() / 20) {
		hash += this->pixels[i].R;
		hash += this->pixels[i].G;
		hash += this->pixels[i].B;
	}
}

UHandTrackerComponent::UHandTrackerComponent() : frames(new TCircularQueue<CameraFrame>(3))
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
	if (rgbArray) {
		delete[] rgbArray;
		rgbArray = nullptr;
	}
}

// Called when the game starts
void UHandTrackerComponent::BeginPlay()
{
	Super::BeginPlay();
	mediaTexture->GetMediaPlayer()->OnPlaybackResumed.AddDynamic(this, &UHandTrackerComponent::OnPlayingVideo);
}

void UHandTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	processCameraFrame();

	if (left.Num() > 3 && left[0].gesture == left[1].gesture && left[2].gesture == left[3].gesture && left[1].gesture != left[2].gesture) {
		LeftHandGestureEvent.Broadcast(left[2].gesture);
	}
	if (right.Num() > 3 && right[0].gesture == right[1].gesture && right[2].gesture == right[3].gesture && right[1].gesture != right[2].gesture) {
		RightHandGestureEvent.Broadcast(right[2].gesture);
	}
}

HandData UHandTrackerComponent::pollHandDataLeft() const
{
	if (left.Num() == 0) return HandData();
	FVector loc = left[0].transform.GetLocation();
	FQuat rot = left[0].transform.GetRotation();
	int numElems = left.Num() - 2;
	float ratio = 1.0f / numElems;
	for (int i = 1; i < left.Num() - 1; ++i) {
		loc = FMath::Lerp<FVector, float>(loc, left[i].transform.GetLocation(), ratio);
		rot = FQuat::Slerp(rot, left[i].transform.GetRotation(), ratio);
	}
	HandData data;
	data.transform = FTransform(rot, loc, FVector(1, 1, 1));
	data.handedness = Handedness::LEFT;
	return data;
}

HandData UHandTrackerComponent::pollHandDataRight() const
{
	if (right.Num() == 0) return HandData();
	FVector loc = right[0].transform.GetLocation();
	FQuat rot = right[0].transform.GetRotation();
	int numElems = right.Num() - 2;
	float ratio = 1.0f / numElems;
	for (int i = 1; i < right.Num() - 1; ++i) {
		loc = FMath::Lerp<FVector, float>(loc, right[i].transform.GetLocation(), ratio);
		rot = FQuat::Slerp(rot, right[i].transform.GetRotation(), ratio);
	}
	HandData data;
	data.transform = FTransform(rot, loc, FVector(1, 1, 1));
	data.handedness = Handedness::RIGHT;
	return data;
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
	if (stripAlphaChannel) {
		rgbArray = new uint8_t[targetCameraWidth * targetCameraHeight * 3];
	}
	_ump = MakeUnique<ump::UMediapipe>(handCallback, targetCameraWidth, targetCameraHeight);
	_ump->beginHandDetection();
}

void UHandTrackerComponent::processCameraFrame()
{
	if (!cameraTextureRT) return;
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, cameraTextureRT, cameraTextureMaterial);

	/*ENQUEUE_RENDER_COMMAND(ReadCameraTexture)(
		[this](FRHICommandListImmediate& RHICmdList) {
			auto* rt = cameraTextureRT->GetRenderTargetResource();*/
	auto* rt = cameraTextureRT->GameThread_GetRenderTargetResource();
	if (rt) {
		TArray<FColor> pixels;
		pixels.AddUninitialized(targetCameraHeight * targetCameraWidth);
		if (rt->ReadPixels(pixels)) {
			CameraFrame frame(std::move(pixels));
			const auto oldFrame = frames->Peek();
			if (!oldFrame || !(frame == *oldFrame)) {
				frames->Enqueue(std::move(frame));
			}
		}
	}
	//}
//);

	CameraFrame frame;
	bool hasFrame = frames->Dequeue(frame);
	if (hasFrame) {
		if (stripAlphaChannel) {
			int it = 0;
			for (int i = 0; i < frame.getPixels().Num(); ++i) {
				rgbArray[it++] = frame.getPixels()[i].R;
				rgbArray[it++] = frame.getPixels()[i].G;
				rgbArray[it++] = frame.getPixels()[i].B;
			}
			_ump->sendFrame(rgbArray);
		}
		else
		{
			_ump->sendFrame((uint8_t*)frame.getPixels().GetData());
		}
	}
}
