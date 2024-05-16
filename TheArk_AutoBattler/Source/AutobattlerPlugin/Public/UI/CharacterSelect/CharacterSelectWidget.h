// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AutobattlerStructs.h"
#include "CharacterSelectWidget.generated.h"

class UButton;
class UCharacterPanel;
class UImage;
class UTextBlock;

/**
 * Widget which allows the selection of a character to deploy.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Button which attempts to deploy this character. */
	UPROPERTY(meta = (BindWidget))
	UButton* DeployButton;

	/* Image of this character. */
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterImage;

	/* Name of this character. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterName;

	/* How expensive this character is budget wise. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterCost;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* ID of character represented by this widget. */
	int32 RepresentedCharacterID;

	/* Owner ID of character represented by this widget. */
	EEntity WhoOwnsInternal;

	/* Lisiting representing character. */
	FCharacterListing InternalListing;

	/* Panel to show more data about character. */
	UCharacterPanel* CharacterPanel;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Builds this selection widget from properties.
	 * @param NewRepresentedCharacterID Character ID of character represented by this widget.
	 * @param WhoOwns Owner ID of character represented by this widget.
	 * @param NewCharacterListing Listing character is to be constructed from.
	 */
	void BuildCharacterSelectWidget(int32 NewRepresentedCharacterID, EEntity WhoOwns, const FCharacterListing& NewCharacterListing);

protected:
	/**
	 * Used to bind delegates.
	 */
	virtual void NativeConstruct() override;

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Gets the represented character's ID.
	 * @return The represented character's ID.
	 */
	int32 GetRepresentedCharacterID() const { return RepresentedCharacterID; }

	/**
	 * Sets deployment button enabled state.
	 * @param Enable True enables, false disables.
	 */
	void SetDeployButtonEnabled(bool Enable);

/////////////////////////////////////////////////////////////////////////////////
//// DELEGATE BOUND
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Tries floating the character.
	 */
	UFUNCTION()
	void OnDeployButtonClicked();

	/**
	 * Shows a character panel for more information. 
	 */
	UFUNCTION()
	void OnDeployButtonMouseOverBegin();

/////////////////////////////////////////////////////////////////////////////////
//// UTILITY
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Attempts to get character DT from autobattler manager.
	 * @return Character DT or null if chain of references failed.
	 */
	const UDataTable* GetCharactersDatatableFromManager() const;
};
