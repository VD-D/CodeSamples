// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModifierText.generated.h"

class URichTextBlock;

/**
 * Widget intended to be used as a tooltip for the modifier icon.
 * Gives a brief description of what the modifier does.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UModifierText : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Brief description of what the modifier does. */
	UPROPERTY(meta = (BindWidget))
	URichTextBlock* DescriptionText;

public:
	/**
	 * Changes modifier description.
	 * @param NewRichTextDescription New description of the modifier
	 */
	void UpdateModifierDescription(const FText& NewRichTextDescription);
};
