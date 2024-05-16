// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerAbilityContainer.generated.h"

class UWrapBox;
class UPlayerAbilityWidget;

/**
 * Widget which serves as a container for a player's abilities.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UPlayerAbilityContainer : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Wrap box in which player abilities are stored. */
	UMETA(meta = (BindWidget))
	UWrapBox* AbilityContainer;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Class to make player ability widget from. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Ability Container")
	TSubclassOf<UPlayerAbilityWidget> PlayerAbilityWidgetClass;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* All currently spawned player ability widgets. */
	UPROPERTY()
	TArray<UPlayerAbilityWidget*> PlayerAbilityWidgets;

	/* Whether abilities are currently enabled or disabled. */
	bool AbilitiesEnabledState;

public:
	/**
	 * Whether to enable or disable player abilities.
	 * @param NewEnabled True allows abilities to be used, false disables them.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler UI")
	void SetPlayerAbilitiesEnabled(bool NewEnabled);

	/**
	 * Retrieves list of current player abilities and updates the container to reflect it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler UI")
	void UpdatePlayerAbilities();
};
