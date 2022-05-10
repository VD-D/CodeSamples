// Copyright Robert Zygmunt Uszynski 2021-2022

#pragma once

#include "CoreMinimal.h"
#include "Slate/WidgetTransform.h"
#include "IconCreatorSettings.generated.h"

USTRUCT(BlueprintType)
struct FIconCreatorSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* BackgroundStyleSheet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BackgroundStyleSheetRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableOutlineInner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableOutlineOuter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableGlassMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseIconText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OverallAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D OverallScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWidgetTransform IconTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="!bUseIconText"))
	FLinearColor IconColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="!bUseIconText"))
	class UTexture2D* IconImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bUseIconText"))
	FText IconText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bUseIconText"))
	FSlateColor IconTextColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bUseIconText"))
	FSlateFontInfo IconTextFont;

	FIconCreatorSettings()
	{
		BackgroundStyleSheetRowName = FName("Default");
		bEnableBackground = true;
		bEnableOutlineInner = true;
		bEnableOutlineOuter = true;
		bEnableMask = true;
		bEnableGlassMask = true;
		OverallAngle = 0.0f;
		OverallScale = FVector2D(1.0f, 1.0f);
	}
};
