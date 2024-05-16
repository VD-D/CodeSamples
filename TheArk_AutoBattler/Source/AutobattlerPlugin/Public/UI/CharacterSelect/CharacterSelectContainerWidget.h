// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AutobattlerStructs.h"
#include "CharacterSelectContainerWidget.generated.h"

class UCharacterSelectWidget;
class UVerticalBox;

/**
 * Widget which serves as a HUD for the "barracks" (i.e. character selection for a given player).
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UCharacterSelectContainerWidget : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Class to use when generating a character select widget. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration", meta = (BlueprintBaseOnly))
	TSubclassOf<UCharacterSelectWidget> CharacterSelectWidgetClass;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Constructed character select widget. */
	UPROPERTY()
	TArray<UCharacterSelectWidget*> CharacterSelectWidgets;

/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Box containing character select widgets. */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* CharacterSelectBox;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Builds a new character select widget and adds it to the character select box.
	 * @param NewRepresentedCharacterID Character ID of character represented by this widget.
	 * @param WhoOwns Owner ID of character represented by this widget.
	 * @param NewCharacterListing Listing character is to be constructed from.
	 */
	void BuildCharacterSelectWidget(int32 NewRepresentedCharacterID, EEntity WhoOwns, const FCharacterListing& NewCharacterListing);

	/**
	 * Attempts to remove a character select widget from the character select box.
	 * @param IDToRemove ID of character to remove.
	 */
	void RemoveCharacterSelectWidget(int32 IDToRemove);

	/**
	 * Enables/disables deployment of characters (enables/disables placement button).
	 * @param IsEnabled True enables, false disables.
	 */
	void SetEnableDeployment(bool IsEnabled);
};
