// Copyright Juggler Games 2022 - 2024
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AutobattlerStructs.h"
#include "PlayerAbilityTooltip.generated.h"

class URichTextBlock;
class UTextBlock;

/**
 * Tooltip providing additional info about a player ability.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UPlayerAbilityTooltip : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BIDNINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Text block updated to contain ability name. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AbilityName;

	/* Multiline rich text block updated to contain ability user-friendly description. */
	UPROPERTY(meta = (BindWidget))
	URichTextBlock* AbilityDescription;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Updates tooltip with relevant data.
	 * @param CosmeticData Data to update from.
	 */
	void BuildTooltip(FAutobattlerPlayerAbilityCosmetic CosmeticData);
};
