// Fill out your copyright notice in the Description page of Project Settings.


#include "ExampleActor.h"
#include "ThirdParty/libandroid/include/libandroid.h"

// Sets default values
AExampleActor::AExampleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExampleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExampleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	int n = getNumber();
	const char* text = getText();

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Number is %d"), n), true, FVector2D(3));
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString(text), true, FVector2D(3));
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