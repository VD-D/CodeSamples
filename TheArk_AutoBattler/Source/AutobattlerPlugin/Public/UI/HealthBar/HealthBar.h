// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * Widget intended to be used by a widget component for displaying a unit health bar.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthPercent;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentHealth;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxHealth;

/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Updates this widget with new health values.
	 * @param NewCurrentHealth The new current health value.
	 * @param NewMaxHealth The new max health value.
	 */
	void UpdateHealthValues(float NewCurrentHealth, float NewMaxHealth);
};
