// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../GameplayInterfaces/MouseSelectableInterface.h"
#include "Components/TimelineComponent.h"
#include "Widgets/Layout/Anchors.h"
#include "DetectiveRoomPawn.generated.h"

UENUM(BlueprintType)
enum class EDetectiveRoomViewTarget : uint8
{
	ComputerScreen UMETA(DisplayName = "Computer Screen"),
	Map UMETA(DisplayName = "Map"),
	Neutral UMETA(DisplayName = "Neutral")
};

UCLASS()
class SACRUSMORDUM_API ADetectiveRoomPawn : public APawn, public IMouseSelectableInterface
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- COMPONENTS --- */

	/* Scene Root. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Detective Room Pawn Components")
	class USceneComponent* SceneRoot;

	/* Camera component for viewing scene. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Detective Room Pawn Components")
	class UCameraComponent* Camera;

	/* --- CONFIGURABLE --- */

	/* Map of Name to DetectiveRoomViewTarget. When the given Name is returned from the IMouseSelectableInterface,
	zooms in onto that target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|View Target Config")
	TMap<FName, EDetectiveRoomViewTarget> MapZoomData;

	/* View target when looking at the map. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|View Target Config")
	class AActor* MapViewTarget;

	/* View target when looking at the computer screen. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|View Target Config")
	class AActor* ComputerScreenViewTarget;

	/* Duration which view target should interpolate to target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|View Target Config")
	float BlendDuration;

	/* Relevant only if BlendType is non-linear. Blend Function exponent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|View Target Config")
	float BlendExponent;

	/* When interpolating, if MoveCameraToViewTarget is called while still interpolating, should the
	interpolation start from the current point (true) or the initial point (false)? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|View Target Config")
	bool bLockOutgoing;

	/* What should the blend function be like? Use cubic for smoothest feel. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|View Target Config")
	TEnumAsByte<EViewTargetBlendFunction> BlendType;

	/* Class of the DetectiveRoomReturnWidget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|Widgets|Return Widget")
	TSubclassOf<class UDetectiveRoomReturnWidget> ReturnWidgetClass;

	/* Viewport ZOrder of the DetectiveRoomReturnWidget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|Widgets|Return Widget")
	int32 ReturnWidgetZOrder;

	/* When clicking on an object, will only try to zoom in on it if its ID == ZoomableID. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|Selectable Meta Info Config")
	int32 ZoomableID;

	/* When clicking on an object, will only try to execute functions relevant to post level streaming if ID == LevelStreamID. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|Selectable Meta Info Config")
	int32 LevelStreamID;

	/* When clicking on an object, will only try to execute functions relevant to post level streaming if ID == LevelStreamID. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detective Room Pawn|Save & Load Config")
	FString SaveSlotName;

private:
	/* Used internally by timers. */
	bool bCanTransition;

	/* Whatever the current view target is. */
	EDetectiveRoomViewTarget CurrentViewTarget;

	/* Internal value used to determine what the last set ID was.*/
	int32 LastID;

	/* Internal value used to determine what the loading level. */
	FName LoadingLevelName;

/* --- FUNCTIONS --- */
public:
	/** Constructor.
	  */
	ADetectiveRoomPawn();

	/** Moves camera to a given position.
	  * @param ViewTarget - The view target to move to
	  * @warning May fail if pointers to view target actors are null.
	  */
	UFUNCTION(BlueprintCallable, Category = "Detective Room Pawn")
	void MoveCameraToViewTarget(EDetectiveRoomViewTarget ViewTarget);

	/** Gets the current view target.
	  * @return The current view target.
	  */
	UFUNCTION(BlueprintCallable, Category = "Detective Room Pawn")
	EDetectiveRoomViewTarget GetCurrentViewTarget();

	/** Transitions from 
	  */

protected:
	/* --- MOUSE SELECTABLE INTERFACE --- */

	/** Response to mouse over dependent upon MouseSelectableMetaInfo.
	  * @param TouchedComponent - Component touched by the mouse.
	  * @param MouseSelectableInfo - Information about the mouse selectable.
	  */
	virtual void OnMouseSelectableMouseOverStart_Implementation(class UPrimitiveComponent* TouchedComponent, FMouseSelectableMetaInfo MouseSelectableInfo) override;

	/** Called when mouse is clicked on a MouseSelectableObject.
	  * @param TouchedComponent - Component touched by the mouse.
	  * @param PressedKey - The button (or mouse key) pressed to trigger the click.
	  * @param MouseSelectableInfo - Information about the mouse selectable.
	  */
	virtual void OnMouseSelectableMouseClicked_Implementation(class UPrimitiveComponent* TouchedComponent, FKey PressedKey, FMouseSelectableMetaInfo MouseSelectableInfo) override;

	/** Called when mouse over ends on a MouseSelectableObject.
	  * @param TouchedComponent - Component touched by the mouse.
	  * @param MouseSelectableInfo - Information about the mouse selectable.
	  */
	virtual void OnMouseSelectableMouseOverEnd_Implementation(class UPrimitiveComponent* TouchedComponent, FMouseSelectableMetaInfo MouseSelectableInfo) override;

private:
	/** Whenever moving camera with MoveCameraToViewTarget, this function is called after it has finished moving. 
	  * @param FinalViewTarget - The final view target being "looked" at.
	  */
	UFUNCTION()
	void OnViewTargetFinishedBlend(EDetectiveRoomViewTarget FinalViewTarget);

	/** Creates a USacrusMordumScenario widget, adds it to the viewport and set this as the DetectiveRoomPawn.
	  */
	void ConfigureSubLevelStreamLoad();

	/** Called through ConfigureSubLevelStreamLoad.
	  * @param SlotName - Name of the file.
	  * @param UserIndex - User to load for.
	  * @param SaveGameData - Data which has been loaded.
	  */
	UFUNCTION()
	void OnConfiguredSaveDataLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	/** Utility function for disabling or enabling highlights on listening actors.
	  * @param Enable - Should highlights be enabled or disabled?
	  */
	void ExecuteHighlightableInterface(bool Enable);
};
