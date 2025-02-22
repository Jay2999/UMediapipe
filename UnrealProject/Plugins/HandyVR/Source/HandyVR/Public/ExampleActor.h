// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ThirdParty/libHandyVR/include/UMediapipe.h"
#include "ExampleActor.generated.h"

UCLASS()
class HANDYVR_API AExampleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExampleActor();
	~AExampleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Plane;

private:
	TWeakObjectPtr<UTextureRenderTarget2D> cameraTextureRT;
	uint32 cameraWidth;
	uint32 cameraHeight;
	TArray<FColor> pixels;
	unsigned char* rgbArray = nullptr;
	UMediapipe::UMediapipe* ump = nullptr;
};
