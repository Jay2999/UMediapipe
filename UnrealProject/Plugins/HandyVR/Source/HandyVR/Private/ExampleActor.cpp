// Fill out your copyright notice in the Description page of Project Settings.


#include "ExampleActor.h"
#include "Components/StaticMeshComponent.h"
#include <Kismet/KismetRenderingLibrary.h>

float lx = 0;
float rx = 0;
void leftHandCallback(UMediapipe::HandLandmarks* landmarks) {
	lx = (*landmarks)[0].x();
}
void rightHandCallback(UMediapipe::HandLandmarks* landmarks) {
	rx = (*landmarks)[0].x();
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
		static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
		if (PlaneFinder.Succeeded()) {
			Plane->SetStaticMesh(PlaneFinder.Object);
		}
		/*static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/HandyVR/CameraTextureMaterial.CameraTextureMaterial"));
		if (MaterialFinder.Succeeded()) {
			Plane->SetMaterial(0, MaterialFinder.Object);
		}*/
	}

	/*cameraTextureRT = NewObject<UTextureRenderTarget2D>(GetTransientPackage(),
		MakeUniqueObjectName(GetTransientPackage(), UTextureRenderTarget2D::StaticClass()));
	cameraTextureRT->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
	cameraTextureRT->ClearColor = FLinearColor::Black;
	cameraTextureRT->bAutoGenerateMips = false;
	cameraTextureRT->InitAutoFormat(500, 500);
	cameraTextureRT->UpdateResourceImmediate(true);*/
	cameraWidth = 1280;
	cameraHeight = 720;
	pixels.AddUninitialized(cameraWidth * cameraHeight);
	rgbArray = new unsigned char[cameraWidth * cameraHeight * 3];
	ump = new UMediapipe::UMediapipe(leftHandCallback, rightHandCallback, cameraWidth, cameraHeight);
}

AExampleActor::~AExampleActor()
{
	ump->stopLandmarkDetection();
	delete ump;
	ump = nullptr;
	delete[] rgbArray;
	rgbArray = nullptr;
}

// Called when the game starts or when spawned
void AExampleActor::BeginPlay()
{
	Super::BeginPlay();
	cameraTextureRT = UKismetRenderingLibrary::CreateRenderTarget2D(this, cameraWidth, cameraHeight, RTF_RGBA8);
	ump->beginLandmarkDetection();
}

// Called every frame
void AExampleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Plane->GetMaterial(0)) {
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
		ump->sendFrame(rgbArray);
	}

	if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Number is %d"), n), true, FVector2D(-3, 3));
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString(text), true, FVector2D(-3, 3));
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("X = %f"), lx), true, FVector2D(6, -6));
		UE_LOG(LogTemp, Log, TEXT("X = %f"), lx);
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("P = %d"), pixels[0].R), true);
	}
}