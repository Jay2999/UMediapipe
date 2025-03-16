// Fill out your copyright notice in the Description page of Project Settings.


#include "ExampleActor.h"
#include "Components/StaticMeshComponent.h"
#include <Kismet/KismetRenderingLibrary.h>

float lx = 0;
FString label = "";
void handCallback(ump::HandLandmarks* landmarks) {
	lx = (*landmarks)[0].x();
	label = landmarks->getHandedness() == ump::Handedness::LEFT ? "Left" : "Right";
}

// Sets default values
AExampleActor::AExampleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	if (Plane) {
		Plane->SetupAttachment(RootComponent);
#if ANDROID
		Plane->SetRelativeScale3D(FVector(-1, 1, 1));
#endif
		static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
		if (PlaneFinder.Succeeded()) {
			Plane->SetStaticMesh(PlaneFinder.Object);
		}
	}

	cameraWidth = 640;
	cameraHeight = 480;
	pixels.AddUninitialized(cameraWidth * cameraHeight);
	rgbArray = new unsigned char[cameraWidth * cameraHeight * 3];
	_ump = new ump::UMediapipe(handCallback, cameraWidth, cameraHeight);
}

AExampleActor::~AExampleActor()
{
	_ump->stopLandmarkDetection();
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
	_ump->beginLandmarkDetection();
}

// Called every frame
void AExampleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Plane->GetMaterial(0) && cameraTextureRT.IsValid()) {
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(Plane, cameraTextureRT.Get(), Plane->GetMaterial(0));
		FRenderTarget* rt = cameraTextureRT->GameThread_GetRenderTargetResource();
		//cameraTextureRT->Resource->TextureRHI->GetTexture2D()->GetNativeResource(); // native texture
		rt->ReadPixels(pixels);

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
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, label, true, FVector2D(6, -6));
#endif
		//UE_LOG(LogTemp, Log, TEXT("X = %f"), lx);
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("P = %d"), pixels[0].R), true);
	}
}