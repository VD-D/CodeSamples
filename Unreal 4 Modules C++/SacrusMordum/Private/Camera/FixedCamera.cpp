// Copyright Robert Zygmunt Uszynski 2020-2021

#include "..\..\Public\Camera\FixedCamera.h"
#include "..\..\Public\Camera\FixedCameraDelegator.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"

AFixedCamera::AFixedCamera()
{
	/* Component creation and setup. */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	RootComponent = SceneRoot;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	/* Tick not required. */
	PrimaryActorTick.bCanEverTick = false;

	/* Reasonable defaults for view rotation on camera. */
	PitchRotationMin = -90.0f;
	PitchRotationMax = 90.0f;
	YawRotationMin = -90.0f;
	YawRotationMax = 90.0f;

	/* Defaults of next/prev camera indices set to -1. */
	NextCameraIndex = -1;
	PrevCameraIndex = -1;
}

void AFixedCamera::NextCamera()
{
	/* Fixed camera delegator needs to be valid for this function to work. Log and return if it is not valid. */
	if (FixedCameraDelegator == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : FixedCameraDelegator pointer invalid!"), *GetDebugName(this)));
		return;
	}

	/* We assume the convention that indexes cannot be less than 0. */
	if (NextCameraIndex < 0)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : When switching to next camera, index is less than 0!"), *GetDebugName(this)));
		return;
	}

	/* Try switch the camera to the next camera. Note that this still may fail if NextCameraIndex is not a valid index in CamerasMap. */
	FixedCameraDelegator->SwitchCamera(NextCameraIndex);
}

void AFixedCamera::PreviousCamera()
{
	/* Fixed camera delegator needs to be valid for this function to work. Log and return if it is not valid. */
	if (FixedCameraDelegator == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : FixedCameraDelegator pointer invalid!"), *GetDebugName(this)));
		return;
	}

	/* We assume the convention that indexes cannot be less than 0. */
	if (PrevCameraIndex < 0)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : When switching to prev camera, index is less than 0!"), *GetDebugName(this)));
		return;
	}

	/* Try switch the camera to the next camera. Note that this still may fail if NextCameraIndex is not a valid index in CamerasMap. */
	FixedCameraDelegator->SwitchCamera(PrevCameraIndex);
}

void AFixedCamera::PossessedBy(AController * NewController)
{
	/* Call super when possessed. */
	Super::PossessedBy(NewController);

	/* See if possessing controller is a player controller. Log and return if it is not. */
	APlayerController* PC = Cast<APlayerController>(NewController);
	if (PC == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : When possessed, passed controller could not be cast to player controller!"), *GetDebugName(this)));
		return;
	}

	/* Get the player camera manager from the PC. */
	APlayerCameraManager* PCM = PC->PlayerCameraManager;

	/* Set min/max rotation values on camera. */
	/*if (PCM != nullptr)
	{
		PCM->ViewPitchMin = PitchRotationMin;
		PCM->ViewPitchMax = PitchRotationMax;
		PCM->ViewYawMin = YawRotationMax;
		PCM->ViewYawMax = YawRotationMin;
	}
	*/
}

void AFixedCamera::BeginPlay()
{
	Super::BeginPlay();
}
