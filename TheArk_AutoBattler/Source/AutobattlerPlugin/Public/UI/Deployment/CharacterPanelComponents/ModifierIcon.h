// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModifierIcon.generated.h"

class UImage;
class UModifierText;
class UProgressBar;
class UWidget;

/**
 * Basic icon representing a modifier which can be applied to a character.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UModifierIcon : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Icon of modifer. */
	UPROPERTY(meta = (BindWidget))
	UImage* ModifierIcon;

	/* Progress bar if modifier is temporary. */
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ModifierDurationCounter;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Class to use for the tooltip description. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	TSubclassOf<UModifierText> ModifierTextWidgetClass;

	/* How quickly the icon progress bar progresses. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	float ModifierIconTickRate = 0.1f;

/////////////////////////////////////////////////////////////////////////////////
//// INTNERAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Text to pass to the tooltip. */
	FText ModifierText;

	/* Used for interpolating progress bar. */
	float CurrentDuration;
	float MaxDuration;

	/* Used for counting down timer. */
	FTimerHandle Handle;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Constructs modifier icon tooltip.
	 * @return Modifier icon tooltip.
	 */
	UFUNCTION()
	UWidget* MakeModifierIconTooltip();

/////////////////////////////////////////////////////////////////////////////////
//// API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Creates this modifier icon.
	 * @param IconImage Image of the modifier.
	 * @param ModifierDescription Rich-text supported description of the modifier.
	 * @param Duration If greater than 0.1, fills out progress bar and removes icon at end of duration.
	 */
	void BuildModifierIcon(UTexture2D* IconImage, const FText& ModifierDescription, float Duration = -1.0f);

private:
	/**
	 * Callback from timer to reduce modifier duration. 
	 */
	UFUNCTION()
	void OnModifierTick();
};
