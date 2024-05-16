// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AutobattlerEnums.h"
#include "AutobattlerDiscreteStatWidget.generated.h"

class UTextBlock;

/**
 * Similar to the stat widget, except it services discrete values rather than numeric ones
 * (e.g. resistance or damage type).
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerDiscreteStatWidget : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Modifier value text representation. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ValueText;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* What stat type this widget represents. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Autobattler")
	EAutobattlerDiscreteType ModifiedStatType;

protected:
	/* Text color when value is at base (not modified). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	FSlateColor BaseColor;

	/* Text color when value has been modified. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	FSlateColor ModifiedColor;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* What the base text is considered to be. */
	FText BaseText;

/////////////////////////////////////////////////////////////////////////////////
//// API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Sets the default "text" value.
	 * @param NewBaseText New default text value.
	 */
	void SetBaseText(const FText& NewBaseText);

	/**
	 * Actually updates the text in the text block.
	 * @param TextToUpdate Set to base color if this is the base text, modified color otherwise.
	 */
	void UpdateText(const FText& TextToUpdate);

protected:
	/**
	 * Called whenever modifier text is updated.
	 * @param IsBase Whether this was the base value or a modified value.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = Autobattler)	
	void OnModifierTextUpdated(bool IsBase);
	virtual void OnModifierTextUpdated_Implementation(bool IsBase);
};
