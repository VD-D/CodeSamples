// Copyright Robert Zygmunt Uszynski 2020-2021

#include "DetectiveRoom/DetectiveRoomPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"

#include "../../Public/Widgets/DetectiveRoomReturnWidget.h"
#include "../../Public/Interfaces/HighlightableInterface.h"
#include "../../Public/SaveGameRelated/SacrusMordumSaveGame.h"

ADetectiveRoomPawn::ADetectiveRoomPawn()
{
	/* Tick required to use SetViewTargetWithBlend. */
	PrimaryActorTick.bCanEverTick = true;

	/* Component creation. */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	/* Reasonable defaults for camera view target blending. */
	BlendDuration = 1.5f;
	BlendExponent = 100.0f;
	bLockOutgoing = true;
	BlendType = EViewTargetBlendFunction::VTBlend_Cubic;

	/* Transitioning enabled by default. */
	bCanTransition = true;
	/* Player is in neutral position by default. */
	CurrentViewTarget = EDetectiveRoomViewTarget::Neutral;

	/* Reasonable defaults for MapZoomData. */
	MapZoomData.Add(FName("Map"), EDetectiveRoomViewTarget::Map);
	MapZoomData.Add(FName("ComputerScreen"), EDetectiveRoomViewTarget::ComputerScreen);

	/* Reasonable defaults for ReturnWidget confing. */
	ReturnWidgetZOrder = 1;

	/* Defaults for various ID configurations. */
	ZoomableID = 0;
	LevelStreamID = 1;

	/* Default save slot name. */
	SaveSlotName = FString("SacrusMordumSaveGame");
}

void ADetectiveRoomPawn::MoveCameraToViewTarget(EDetectiveRoomViewTarget ViewTarget)
{
	/* If transitioning or already at the current view target, do nothing. */
	if (!bCanTransition || ViewTarget == CurrentViewTarget) return;

	/* If the player controller is invalid, do nothing. */
	APlayerController* PC = GetController<APlayerController>();
	if (PC == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : Does not have player controller!"), *GetDebugName(this)));
		return;
	}

	/* Set the view target actor by the desired ViewTarget. */
	AActor* ViewTargetActor = nullptr;
	switch (ViewTarget)
	{
	case EDetectiveRoomViewTarget::ComputerScreen:
		ViewTargetActor = ComputerScreenViewTarget;
		break;

	case EDetectiveRoomViewTarget::Map:
		ViewTargetActor = MapViewTarget;
		break;

	case EDetectiveRoomViewTarget::Neutral:
		ViewTargetActor = this;
		break;
	}

	/* If the view target is not set (e.g. not set by the user), then do nothing further. */
	if (ViewTargetActor == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : Failed to set view target actor!"), *GetDebugName(this)));
		return;
	}

	/* On success (both PC and ViewTargetActor are valid), first prevent further transition and set the view target with blend. */
	bCanTransition = false;
	PC->SetViewTargetWithBlend(ViewTargetActor, BlendDuration, BlendType.GetValue(), BlendExponent, bLockOutgoing);

	/* Then set up a timer which triggers at the end of the blend duration. */
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	TimerDel.BindUFunction(this, FName("OnViewTargetFinishedBlend"), ViewTarget);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, BlendDuration, false);
}

EDetectiveRoomViewTarget ADetectiveRoomPawn::GetCurrentViewTarget()
{
	return CurrentViewTarget;
}

void ADetectiveRoomPawn::OnMouseSelectableMouseOverStart_Implementation(UPrimitiveComponent * TouchedComponent, FMouseSelectableMetaInfo MouseSelectableInfo)
{
	/* Dummy Declaration */
}

void ADetectiveRoomPawn::OnMouseSelectableMouseClicked_Implementation(UPrimitiveComponent * TouchedComponent, FKey PressedKey, FMouseSelectableMetaInfo MouseSelectableInfo)
{
	if (MouseSelectableInfo.SelectableItemID == ZoomableID)
	{
		/* Case when the player wants to zoom in onto the object they are clicking on. */
		EDetectiveRoomViewTarget* NewViewTarget = MapZoomData.Find(MouseSelectableInfo.SelectableItemName);
		if (NewViewTarget != nullptr)
		{
			LastID = ZoomableID;
			MoveCameraToViewTarget(*NewViewTarget);
			ExecuteHighlightableInterface(false);
		}
	}
	else if (MouseSelectableInfo.SelectableItemID == LevelStreamID)
	{
		/* Case when a level is being loaded and so appropriate actions need to happen. */
		LastID = LevelStreamID;
		LoadingLevelName = MouseSelectableInfo.SelectableItemName;
		MoveCameraToViewTarget(EDetectiveRoomViewTarget::ComputerScreen);
	}
}

void ADetectiveRoomPawn::OnMouseSelectableMouseOverEnd_Implementation(UPrimitiveComponent * TouchedComponent, FMouseSelectableMetaInfo MouseSelectableInfo)
{
	/* Dummy Declaration */
}

void ADetectiveRoomPawn::OnViewTargetFinishedBlend(EDetectiveRoomViewTarget FinalViewTarget)
{
	/* Allow transitioning if view target has finished blending. Update view target. */
	bCanTransition = true;
	CurrentViewTarget = FinalViewTarget;

	/* Re-enable highlighting upon returning to neutral position. */
	if (CurrentViewTarget == EDetectiveRoomViewTarget::Neutral) ExecuteHighlightableInterface(true);

	if (CurrentViewTarget == EDetectiveRoomViewTarget::ComputerScreen || CurrentViewTarget == EDetectiveRoomViewTarget::Map)
	{
		UDetectiveRoomReturnWidget* ReturnWidget = CreateWidget<UDetectiveRoomReturnWidget>(GetWorld(), ReturnWidgetClass);
		if (ReturnWidget != nullptr)
		{
			ReturnWidget->SetDetectiveRoomPawn(this);
			ReturnWidget->AddToViewport(ReturnWidgetZOrder);
		}
		else
		{
			if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : Failed to create Return Widget!"), *GetDebugName(this)));
		}
	}

	if (LastID == LevelStreamID) ConfigureSubLevelStreamLoad();
}

void ADetectiveRoomPawn::ConfigureSubLevelStreamLoad()
{
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUFunction(this, FName("OnConfiguredSaveDataLoaded"));
	UGameplayStatics::AsyncLoadGameFromSlot(SaveSlotName, 0, LoadedDelegate);
}

void ADetectiveRoomPawn::OnConfiguredSaveDataLoaded(const FString & SlotName, const int32 UserIndex, USaveGame * LoadedGameData)
{
	USacrusMordumSaveGame* SacrusMordumSaveGame = Cast<USacrusMordumSaveGame>(LoadedGameData);
	if (SacrusMordumSaveGame == nullptr)
	{
		// Error message here
		return;
	}

	TArray<FSubLevelUnlocked> SaveDataUnlockedSublevels;
	SacrusMordumSaveGame->GetUnlockedSublevels(LoadingLevelName, SaveDataUnlockedSublevels);
}

void ADetectiveRoomPawn::ExecuteHighlightableInterface(bool Enable)
{
	TArray<AActor*> ListeningActors;
	UGameplayStatics::GetAllActorsWithInterface(this, UHighlightableInterface::StaticClass(), ListeningActors);

	for (auto ListeningActor : ListeningActors)
	{
		IHighlightableInterface* I = Cast<IHighlightableInterface>(ListeningActor);
		if (I != nullptr) I->Execute_SetEnableHighlights(ListeningActor, Enable);
	}
}
