// Copyright Robert Zygmunt Uszynski 2020-2021

#include "Widgets/InvestigationInfoWidget.h"
#include "Components/ScrollBox.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameplayActors/InvestigationObject.h"
#include "..\..\Public\Widgets\InvestigationInfoWidget.h"
#include "../../SacrusMordum.h"

void UInvestigationInfoWidget::UpdateInvestigationInfo(FInvestigationObjectProperties NewInvestigationObjectProperties)
{
	/* Call PreInformationUpdated to allow any pre-updates to complete. */
	PreInformationUpdated();

	/* Set new object name from variable format string. */
	ObjectName = VariableFormatString(NewInvestigationObjectProperties.ObjectName.BaseString, NewInvestigationObjectProperties.ObjectName.Format);

	/* Set investigation info based on new information. Empty current ObjectInformation array. Call OnInformationUpdated each time next text is added. */
	ObjectInformation.SetNum(0);

	for (auto NewInfo : NewInvestigationObjectProperties.ObjectInformation)
	{
		FText NewText = VariableFormatString(NewInfo.BaseString, NewInfo.Format);
		ObjectInformation.Add(NewText);
		OnInformationUpdated(NewText, NewInfo.Tags);
	}

	/* Set the image resource to be renedered by the slate brush. */
	ImageInfo.SetResourceObject(Cast<UObject>(NewInvestigationObjectProperties.ObjectDisplayRender));
	ImageInfo.SetImageSize(FVector2D(ImageXSize, ImageYSize));
}

void UInvestigationInfoWidget::ShouldEnterTransitionAnimation_Implementation(bool Direction)
{
	/* This function should be overriden. Log a warning if this function has been reached. */
	UE_LOG(LogSacrusMordum, Warning, TEXT("Reached non-overriden declaration of ShouldEnterTransitionAnimation: Please override this function!"))
}

void UInvestigationInfoWidget::UpdateScrollBarOffset_Implementation(float AxisValue)
{
	/* Dummy declaration */
}

void UInvestigationInfoWidget::PreInformationUpdated_Implementation()
{
	/* Dummy declaration */
}

void UInvestigationInfoWidget::OnInformationUpdated_Implementation(const FText& NewInformation, const TArray<FName>& Tags)
{
	/* Dummy declaration */
}

void UInvestigationInfoWidget::OnFocusBegan_Implementation()
{
	/* Dummy declaration */
}

void UInvestigationInfoWidget::OnFocusInterrupted_Implementation()
{
	/* Dummy declaration */
}

void UInvestigationInfoWidget::OnFocusCompleted_Implementation()
{
	/* Dummy declaration */
}

void UInvestigationInfoWidget::OnInvestigationObjectFound_Implementation(AInvestigationObject * FoundObject)
{
	if (FoundObject == nullptr)
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Investigation Info Widget: Passed FoundObject was nullptr!"))
		return;
	}

	UpdateInvestigationInfo(FoundObject->GetInvestigationObjectInformation());
}

void UInvestigationInfoWidget::OnInvestigationObjectLost_Implementation(AInvestigationObject * LostObject)
{
	/* Dummy declaration */
}

void UInvestigationInfoWidget::UpdateScrollBar(UScrollBox * ScrollBar, float AxisValue, float SpeedMultiplier, bool UseInterpolation, float InterpolationSpeed)
{
	/* Do nothing if the scoll bar is invalid. */
	if (!IsValid(ScrollBar)) return;

	/* Compute the scroll offset, ensuring that it is between 0.0f and the max scroll offset to prevent "overscrolling". */
	float NewScrollOffset = (AxisValue * -1.0f * SpeedMultiplier) + ScrollBar->GetScrollOffset();
	NewScrollOffset = FMath::Clamp(NewScrollOffset, 0.0f, ScrollBar->GetScrollOffsetOfEnd());

	/* If using interpolation, interpolate to the new scroll offset, but only if GetWorld() is not null. */
	if (UseInterpolation && GetWorld() != nullptr)
	{
		ScrollBar->SetScrollOffset(FMath::FInterpTo(ScrollBar->GetScrollOffset(), NewScrollOffset, GetWorld()->GetDeltaSeconds(), InterpolationSpeed));
	}
	/* If not using interpolation, simply set the scroll bar's offset to the new scroll offset.*/
	else
	{
		ScrollBar->SetScrollOffset(NewScrollOffset);
	}
}

FText UInvestigationInfoWidget::VariableFormatString(FString Base, const TArray<FString>& FormatArray)
{
	if (FormatArray.Num() == 0) return FText::FromString(Base);

	/* Create an array of format args. */
	TArray<FStringFormatArg> FormatArgs;

	for (auto Format : FormatArray)
	{
		FormatArgs.Add(FStringFormatArg(Format));
	}

	/* Return the formatted string. */
	return FText::FromString(FString::Format(*Base, FormatArgs));
}
