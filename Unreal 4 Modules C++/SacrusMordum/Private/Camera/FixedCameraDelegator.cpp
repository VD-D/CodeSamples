// Copyright Robert Zygmunt Uszynski 2020-2021

#include "..\..\Public\Camera\FixedCameraDelegator.h"
#include "..\..\Public\Camera\FixedCamera.h"

#include "Components/BillboardComponent.h"

AFixedCameraDelegator::AFixedCameraDelegator()
{
	/* Component creation. */
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	RootComponent = Billboard;

	/* Tick not required. */
	PrimaryActorTick.bCanEverTick = false;

	/* Assume that we want to possess on begin play. */
	bAutoPossessOnBeginPlay = true;

	/* Camera Delegator default name. */
	CameraDelegatorName = FName("CameraDelegator");
}

void AFixedCameraDelegator::SwitchCamera(int32 KeyIndex)
{
	if (GetWorld() == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : This actor is not in a world!"), *GetDebugName(this)));
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AFixedCamera* NewFixedCamera = *CamerasMap.Find(KeyIndex);

	if (NewFixedCamera == nullptr || PC == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : Could not find camera at KeyIndex = %d or player controller invalid!"), *GetDebugName(this), KeyIndex));
		return;
	}

	PC->Possess(NewFixedCamera);
}

void AFixedCameraDelegator::BeginPlay()
{
	Super::BeginPlay();
	
	if (bAutoPossessOnBeginPlay) SwitchCamera(StartKey);
}
