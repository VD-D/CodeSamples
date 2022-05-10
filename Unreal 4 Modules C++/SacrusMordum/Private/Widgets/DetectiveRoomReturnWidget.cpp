// Copyright Robert Zygmunt Uszynski 2020-2021

#include "Widgets/DetectiveRoomReturnWidget.h"
#include "../../Public/DetectiveRoom/DetectiveRoomPawn.h"

void UDetectiveRoomReturnWidget::SetDetectiveRoomPawn(ADetectiveRoomPawn * NewDetectiveRoomPawn)
{
	DetectiveRoomPawn = NewDetectiveRoomPawn;
}

void UDetectiveRoomReturnWidget::PlayDetectiveRoomPawnReturnTransition()
{
	if (DetectiveRoomPawn != nullptr)
	{
		DetectiveRoomPawn->MoveCameraToViewTarget(EDetectiveRoomViewTarget::Neutral);
		RemoveFromViewport();
	}
}