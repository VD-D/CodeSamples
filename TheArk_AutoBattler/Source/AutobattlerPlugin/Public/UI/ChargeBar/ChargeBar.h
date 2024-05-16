// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChargeBar.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * Charge bar which displays a character's number of charges.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UChargeBar : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ChargePercent;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentCharge;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxCharge;

/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Updates this widget with new health values.
	 * @param NewCurrentHealth The new current health value.
	 * @param NewMaxHealth The new max health value.
	 */
	void UpdateChargeValues(int32 NewCurrentCharge, int32 NewMaxCharge);
};
