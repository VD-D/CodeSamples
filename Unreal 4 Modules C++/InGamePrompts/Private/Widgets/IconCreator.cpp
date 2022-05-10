// Copyright Robert Zygmunt Uszynski 2021-2022

/* Class header */
#include "Widgets/IconCreator.h"

/* Other includes */
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UIconCreator::RebuildIcon()
{
    if (BackgroundStyleSheet == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString("Background Style Sheet for Icon Creator widget is null!"));
        return;
    }

    FIconStyleSheet* IconStyleSheet = BackgroundStyleSheet->FindRow<FIconStyleSheet>(BackgroundStyleSheetRowName, FString("Icon Creator"));
    if (IconStyleSheet == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Cannot find row %s in Icon Creator widget!"), *BackgroundStyleSheetRowName.ToString()));
        return;
    }

    if (bUseIconText)
    {
        if (IconImage != nullptr) IconImage->SetVisibility(ESlateVisibility::Collapsed);
        if (IconText != nullptr) IconText->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    else
    {
        if (IconImage != nullptr) IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
        if (IconText != nullptr) IconText->SetVisibility(ESlateVisibility::Collapsed);
    }

    SetImageOnly(BackgroundImage, IconStyleSheet->Background);
    SetImageOnly(OutlineInnerImage, IconStyleSheet->OutlineInner);
    SetImageOnly(OutlineOuterImage, IconStyleSheet->OutlineOuter);
    SetImageOnly(MaskImage, IconStyleSheet->Mask);
    SetImageOnly(GlassMaskImage, IconStyleSheet->GlassMask);

    SetImageColor(BackgroundImage, IconStyleSheet->BackgroundTint, bEnableBackground);
    SetImageColor(OutlineInnerImage, IconStyleSheet->OutlineInnerTint, bEnableOutlineInner);
    SetImageColor(OutlineOuterImage, IconStyleSheet->OutlineOuterTint, bEnableOutlineOuter);
    SetImageColor(MaskImage, IconStyleSheet->MaskTint, bEnableMask);
    SetImageColor(GlassMaskImage, IconStyleSheet->GlassMaskTint, bEnableGlassMask);

    SetImageScaleAndRotation(BackgroundImage, OverallScale, OverallAngle);
    SetImageScaleAndRotation(OutlineInnerImage, OverallScale, OverallAngle);
    SetImageScaleAndRotation(OutlineOuterImage, OverallScale, OverallAngle);
    SetImageScaleAndRotation(MaskImage, OverallScale, OverallAngle);
    SetImageScaleAndRotation(GlassMaskImage, OverallScale, OverallAngle);
}

void UIconCreator::SetParametersFromSettings(const FIconCreatorSettings& NewSettings)
{
    BackgroundStyleSheet = NewSettings.BackgroundStyleSheet;
    BackgroundStyleSheetRowName = NewSettings.BackgroundStyleSheetRowName;
    bEnableBackground = NewSettings.bEnableBackground;
    bEnableOutlineInner = NewSettings.bEnableOutlineInner;
    bEnableOutlineOuter = NewSettings.bEnableOutlineOuter;
    bEnableMask = NewSettings.bEnableMask;
    bEnableGlassMask = NewSettings.bEnableGlassMask;
    bUseIconText = NewSettings.bUseIconText;
    OverallAngle = NewSettings.OverallAngle;
    OverallScale = NewSettings.OverallScale;

    if (bUseIconText)
    {
        if (IconText != nullptr)
        {
            IconText->SetText(NewSettings.IconText);
            IconText->SetFont(NewSettings.IconTextFont);
            IconText->SetColorAndOpacity(NewSettings.IconTextColor);
            IconText->SetRenderTransform(NewSettings.IconTransform);
        }
    }
    else
    {
        if (IconImage != nullptr)
        {
            IconImage->SetColorAndOpacity(NewSettings.IconColor);
            SetImageOnly(IconImage, NewSettings.IconImage);
            IconImage->SetRenderTransform(NewSettings.IconTransform);
        }
    }
}

void UIconCreator::NativePreConstruct()
{
    RebuildIcon();
}

void UIconCreator::SetImageOnly(UImage* Image, UTexture2D* Texture)
{
    if (!IsValid(Image)) return;

    Image->SetBrushFromTexture(Texture, true);
    // Image->SetBrushResourceObject(Image);
}

void UIconCreator::SetImageColor(UImage* Image, const FLinearColor& Color, bool NotFullyTranslucent)
{
    if (!IsValid(Image)) return;

    if (NotFullyTranslucent) Image->SetBrushTintColor(Color);
    else Image->SetBrushTintColor(FLinearColor::Transparent);
}

void UIconCreator::SetImageScaleAndRotation(class UImage* Image, const FVector2D& Scale, float Rotation)
{
    if (!IsValid(Image)) return;

    Image->SetRenderScale(Scale);
    Image->SetRenderTransformAngle(Rotation);
}