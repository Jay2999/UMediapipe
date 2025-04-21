// Fill out your copyright notice in the Description page of Project Settings.


#include "HandTrackerComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "MediaPlayer.h"

float lx = 0;
static void handCallback(ump::HandDetectionResult* hand) {
	lx = hand->Landmarks()[0].x();
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

// Sets default values for this component's properties
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
	if (stripAlphaChannel) {
		rgbArray = new uint8_t[targetCameraWidth * targetCameraHeight * 3];
	}
	_ump = new ump::UMediapipe(handCallback, targetCameraWidth, targetCameraHeight);
	_ump->beginHandDetection();
}

void UHandTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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

	if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Number is %d"), n), true, FVector2D(-3, 3));
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString(text), true, FVector2D(-3, 3));
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("X = %f"), lx), true, FVector2D(6, -6));
#if WINDOWS
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, label, true);
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("X = %f"), lx), true);
#else
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("X = %f"), aa), true, FVector2D(6, -6));
#endif
		//UE_LOG(LogTemp, Log, TEXT("X = %f"), lx);
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("P = %d"), pixels[0].R), true);
	}
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
}
