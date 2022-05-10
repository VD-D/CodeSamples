// Copyright Robert Zygmunt Uszynski 2021-2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Structures/IconCreatorSettings.h"
#include "IconCreator.generated.h"

USTRUCT(BlueprintType)
struct FIconStyleSheet : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	class UTexture2D* Background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	FLinearColor BackgroundTint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	class UTexture2D* OutlineInner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	FLinearColor OutlineInnerTint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	class UTexture2D* OutlineOuter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	FLinearColor OutlineOuterTint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	class UTexture2D* Mask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	FLinearColor MaskTint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	class UTexture2D* GlassMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Style Sheet")
	FLinearColor GlassMaskTint;
};

/* Class for creating icons. */
UCLASS()
class INGAMEPROMPTS_API UIconCreator : public UUserWidget
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* BackgroundImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* OutlineInnerImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* OutlineOuterImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* MaskImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* GlassMaskImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* IconText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background Style Sheet")
	class UDataTable* BackgroundStyleSheet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background Style Sheet")
	FName BackgroundStyleSheetRowName = FName("Default");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background Style Sheet")
	bool bEnableBackground = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background Style Sheet")
	bool bEnableOutlineInner = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background Style Sheet")
	bool bEnableOutlineOuter = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background Style Sheet")
	bool bEnableMask = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background Style Sheet")
	bool bEnableGlassMask = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Settings General")
	bool bUseIconText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Settings General", meta = (ClampMin="-360", ClampMax="360"))
	float OverallAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon Settings General", meta = (ClampMin="0.1"))
	FVector2D OverallScale = FVector2D(1.0f, 1.0f);

/* --- FUNCTIONS --- */
public:
	/**
	 * Rebuilds the icon from parameters.
	 */
	UFUNCTION(BlueprintCallable, Category = "Icon Creator")
	void RebuildIcon();

	/**
	 * Updates current settings (does not change defaults) to use when rebuilding icon.
	 * Call prior to using RebuildIcon().
	 * @param NewSettings New settings to use.
	 */
	UFUNCTION(BlueprintCallable, Category = "Icon Creator")
	void SetParametersFromSettings(const FIconCreatorSettings& NewSettings);

protected:
	/**
	 * Pre-construct override to rebuild widget in editor.
	 */
	virtual void NativePreConstruct() override;
	
private:
	/**
	 * Utility function to set texture on an image without modifying other properties of the image brush.
	 * @param Image Image to set texture on.
	 * @param Texture Texture to set.
	 */
	void SetImageOnly(class UImage* Image, class UTexture2D* Texture);

	/**
	 * Utility function to set a color on an image, or set it to be fully translucent if NotFullyTranslucent = false.
	 * @param Image Image to set color on
	 * @param Color New color, only if NotFullyTranslucent = true
	 * @param NotFullyTranslucent Sets color to fully translucent if false
	 */
	void SetImageColor(class UImage* Image, const FLinearColor& Color, bool NotFullyTranslucent);

	/**
	 * Utility function to set rotation and scale on image.
	 * @param Image Image to set rotation and scale on.
	 * @param Scale Scale to set.
	 * @param Rotation Rotation to set.
	 */
	void SetImageScaleAndRotation(class UImage* Image, const FVector2D& Scale, float Rotation);
};
