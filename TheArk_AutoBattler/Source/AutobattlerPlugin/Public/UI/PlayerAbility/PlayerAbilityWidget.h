// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AutobattlerStructs.h"
#include "PlayerAbilityWidget.generated.h"

class UButton;
class UImage;
class UPlayerAbilityTooltip;

/**
 * Widget which represents an activeatable player ability.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UPlayerAbilityWidget : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BIDNINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* This button is used to select the player ability. */
	UPROPERTY(meta = (BindWidget))
	UButton* SelectAbility;

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* PlayerAbilityImage;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Class used for player ability tooltip. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	TSubclassOf<UPlayerAbilityTooltip> PlayerAbilityTooltipClass;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Cosmetic data of the player ability to execute. */
	FAutobattlerPlayerAbilityCosmetic AbilityData;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Constructs this widget with data.
	 * @param AbilityData Data which defines cosmetic aspects of this ability.
	 */
	void BuildAbilityWidget(FAutobattlerPlayerAbilityCosmetic NewAbilityData);

	/**
	 * Cosmetic only enable/disable. Sets whether or not the button is clickable.
	 * @param NewEnabled True if ability is enabled, false otherwise.
	 */
	void SetAbilityEnabled(bool NewEnabled);

protected:
	/**
	 * Used to bind delegates.
	 */
	virtual void NativeConstruct() override;

private:
	/**
	 * Tries to execute player ability. 
	 */
	UFUNCTION()
	void OnSelectAbilityButtonPressed();

	/**
	 * Generates the player ability tooltip. 
	 */
	UFUNCTION()
	UWidget* GetAbilityTooltip();
};
