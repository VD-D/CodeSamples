// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AutobattlerEnums.h"
#include "AutobattlerStatWidget.generated.h"

class UTextBlock;

/**
 * Widget which shows a numeric stat type.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerStatWidget : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* The base value for this stat. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatBaseValue;

	/* Modifiers affecting this stat. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatModifierValue;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* What stat type this widget represents. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Autobattler")
	EAutobattlerStatType ModifiedStatType;

protected:
	/* Modifier text color when modifier is positive. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	FSlateColor PositiveColor;

	/* Modifier text color when modifier is negative. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	FSlateColor NegativeColor;

	/* Modifier text color when modifier is zero. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	FSlateColor ZeroColor;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Current value of the modifier. */
	float CurrentModifierValue;

/////////////////////////////////////////////////////////////////////////////////
//// API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Updates the base or modifier value for the state.
	 * @param IsBase If true, will set base value, otherwise will set modifier value.
	 * @param ModifierScale How large the modifier value is.
	 */
	void UpdateValue(bool IsBase, float ModifierScale);

protected:
	/**
	 * Whenever the stat modifier value changes, this function is called.
	 * @param NewStatValue New change value.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler")
	void OnStatModifierChange(float NewStatValue);
	virtual void OnStatModifierChange_Implementation(float NewStatValue);
};
