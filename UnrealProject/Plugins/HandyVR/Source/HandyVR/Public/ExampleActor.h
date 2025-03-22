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

	UFUNCTION(BlueprintCallable)
	FTransform pollTransformLeft() const;

	UFUNCTION(BlueprintCallable)
	FTransform pollTransformRight() const;

private:
	uint32 cameraWidth;
	uint32 cameraHeight;
	TArray<FColor> pixels;
	unsigned char* rgbArray = nullptr;
	ump::UMediapipe* _ump = nullptr;

	UPROPERTY()
	UTextureRenderTarget2D* cameraTextureRT;

	UPROPERTY()
	UMaterial* cameraTextureMaterial = nullptr;
};
