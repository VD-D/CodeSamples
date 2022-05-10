// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectiveRoomReturnWidget.generated.h"

/* Widget to return to initial position after zooming into the map. */
UCLASS()
class SACRUSMORDUM_API UDetectiveRoomReturnWidget : public UUserWidget
{
	GENERATED_BODY()
/* --- VARIABLES --- */
private:
	/* The detective room pawn present in the detective room. */
	class ADetectiveRoomPawn* DetectiveRoomPawn;

/* --- FUNCTIONS --- */
public:
	/** Function which sets the detective room pawn. Should be called before calling PlayReturnTransition.
	  * @param NewDetectiveRoomPawn - The DetectiveRoomPawn to set.
	  */
	void SetDetectiveRoomPawn(class ADetectiveRoomPawn* NewDetectiveRoomPawn);

	/** Function which returns the detective pawn camera to its initial position, e.g. on button pressed.
	  */
	UFUNCTION(BlueprintCallable, Category = "Detective Room Return Widget", meta = (DisplayName = "Detective Pawn Return Transition"))
	void PlayDetectiveRoomPawnReturnTransition();
};
