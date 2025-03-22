// Fill out your copyright notice in the Description page of Project Settings.


#include "ExampleActor.h"
#include "Components/StaticMeshComponent.h"
#include <Kismet/KismetRenderingLibrary.h>

static FVector toUSpace(const ump::Landmark& landmark) {
#if WINDOWS
	float y = 1 - landmark.x() * 2;
	float x = 1 - 2 * landmark.z();
#else
	float y = landmark.x() * 2 - 1;
	float x = 2 * landmark.z() - 1;
#endif
	return FVector(x, y, 1 - 2 * landmark.y());
}

float aa = 0;
static float approxDepth(const ump::HandLandmarks& landmarks) {
	auto l0 = toUSpace(landmarks[0]);
	auto v1 = toUSpace(landmarks[5]) - l0;
	auto v2 = toUSpace(landmarks[17]) - l0;
	float a = v1.Size() + v2.Size();
#if ANDROID
	a = 2.5 - a;
#endif
	aa = 2 * a;
	return a * 2;
}

struct HandTransform {
	FVector location;
	FVector f = FVector(1, 0, 0);
	FVector r = FVector(0, 1, 0);
	FVector u = FVector(0, 0, 1);
};
HandTransform left, right;

FString label;
static void handCallback(ump::HandDetectionResult* hand) {
	ump::HandDetectionResult& r = *hand;
	label = r.getHandedness() == ump::Handedness::LEFT ? "left" : "right";
	HandTransform& target = r.getHandedness() == ump::Handedness::LEFT ? left : right;

	auto l0 = toUSpace(r.Landmarks(0));
	target.location = l0;
	target.location.X = approxDepth(r.Landmarks());

	auto v1 = toUSpace(r.Landmarks(5)) - l0;
	auto v2 = toUSpace(r.Landmarks(17)) - l0;
	if (r.getHandedness() == ump::Handedness::RIGHT) {
		std::swap(v1, v2);
	}
	target.u = FVector::CrossProduct(v1, v2);
	target.u.Normalize();

	target.f = toUSpace(r.Landmarks(9)) + toUSpace(r.Landmarks(13)) - 2 * l0 + v1 + v2;
	target.f /= 4;
	target.f -= FVector::DotProduct(target.f, target.u) * target.u;
	target.f.Normalize();

	target.r = FVector::CrossProduct(target.f, target.u);
}

// Sets default values
AExampleActor::AExampleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/HandyVR/CameraTextureMaterial.CameraTextureMaterial"));
	if (MaterialFinder.Succeeded()) {
		cameraTextureMaterial = MaterialFinder.Object;
	}

	cameraWidth = 640;
	cameraHeight = 480;
	pixels.AddUninitialized(cameraWidth * cameraHeight);
	rgbArray = new unsigned char[cameraWidth * cameraHeight * 3];
	_ump = new ump::UMediapipe(handCallback, cameraWidth, cameraHeight);
}

AExampleActor::~AExampleActor()
{
	_ump->stopHandDetection();
	delete _ump;
	_ump = nullptr;
	delete[] rgbArray;
	rgbArray = nullptr;
}

// Called when the game starts or when spawned
void AExampleActor::BeginPlay()
{
	Super::BeginPlay();
	cameraTextureRT = UKismetRenderingLibrary::CreateRenderTarget2D(this, cameraWidth, cameraHeight, RTF_RGBA8);
	_ump->beginHandDetection();
}

// Called every frame
void AExampleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, cameraTextureRT, cameraTextureMaterial);
	FRenderTarget* rt = cameraTextureRT->GameThread_GetRenderTargetResource();
	//cameraTextureRT->Resource->TextureRHI->GetTexture2D()->GetNativeResource(); // native texture
	rt->ReadPixels(pixels);

	bool isSameFrame = true;
	for (int i = 0; i < pixels.Num(); i += pixels.Num() / 20) {
		uint8 r = rgbArray[i * 3];
		uint8 g = rgbArray[i * 3 + 1];
		uint8 b = rgbArray[i * 3 + 2];
		isSameFrame = isSameFrame && r == pixels[i].R && g == pixels[i].G && b == pixels[i].B;
		if (!isSameFrame) break;
	}

	if (!isSameFrame) {
		int it = 0;
		for (int i = 0; i < pixels.Num(); ++i) {
			rgbArray[it++] = pixels[i].R;
			rgbArray[it++] = pixels[i].G;
			rgbArray[it++] = pixels[i].B;
		}
		_ump->sendFrame(rgbArray);
	}

	if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Number is %d"), n), true, FVector2D(-3, 3));
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString(text), true, FVector2D(-3, 3));
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("X = %f"), lx), true, FVector2D(6, -6));
#if WINDOWS
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, label, true);
#else
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("X = %f"), aa), true, FVector2D(6, -6));
#endif
		//UE_LOG(LogTemp, Log, TEXT("X = %f"), lx);
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("P = %d"), pixels[0].R), true);
	}
}

FTransform AExampleActor::pollTransformLeft() const
{
	return FTransform(left.r, left.f, left.u, left.location);
}

FTransform AExampleActor::pollTransformRight() const
{
	return FTransform(right.r, right.f, right.u, right.location);
}
