// Fill out your copyright notice in the Description page of Project Settings.


#include "ExampleActor.h"
#include "Components/StaticMeshComponent.h"
#include <Kismet/KismetRenderingLibrary.h>
#include "ThirdParty/libandroid/include/libandroid.h"


#ifndef _MSC_VER
#include "ThirdParty/libandroid/include/UMediapipe.h"
#endif // !_MSC_VER


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
}

float x = 0;
#ifndef _MSC_VER
void __cdecl callback(HandLandmarks* landmarks) {
	x = landmarks->values[0].x();
}
#endif // !_MSC_VER

// Called when the game starts or when spawned
void AExampleActor::BeginPlay()
{
	Super::BeginPlay();
#ifndef _MSC_VER
	beginLandmarkDetection(callback);
#endif // !_MSC_VER
	cameraTextureRT = UKismetRenderingLibrary::CreateRenderTarget2D(this, cameraWidth, cameraHeight, RTF_RGBA8);
}

// Called every frame
void AExampleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	int n = getNumber();
	const char* text = getText();

	if (Plane->GetMaterial(0)) {
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(Plane, cameraTextureRT.Get(), Plane->GetMaterial(0));
		FRenderTarget* rt = cameraTextureRT->GameThread_GetRenderTargetResource();
		//cameraTextureRT->Resource->TextureRHI->GetTexture2D()->GetNativeResource(); // native texture
		rt->ReadPixels(pixels);
#ifndef _MSC_VER
		unsigned char* rgb = new unsigned char[cameraWidth * cameraHeight * 3];
		int it = 0;
		for (int i = 0; i < pixels.Num(); ++i) {
			rgb[it++] = pixels[i].R;
			rgb[it++] = pixels[i].G;
			rgb[it++] = pixels[i].B;
		}
		sendFrame(rgb, cameraWidth, cameraHeight);
		delete[] rgb;
#endif // !_MSC_VER
	}

	if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Number is %d"), n), true, FVector2D(-3, 3));
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString(text), true, FVector2D(-3, 3));
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("X = %f"), x), true, FVector2D(6, -6));
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("P = %d"), pixels[0].R), true);
	}
}

#ifdef _MSC_VER
int __cdecl getNumber() {
	return 99;
}
const char* __cdecl getText() {
	static const char* txt = "this is text from unreal (99)";
	return txt;
}
#endif