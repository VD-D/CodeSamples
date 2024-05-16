// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AutobattlerStructs.h"
#include "CharacterPanel.generated.h"

class UImage;
class UModifierIcon;
class USkillIcon;
class UTextBlock;
class UTexture2D;
class UWrapBox;

// USTRUCT(BlueprintType)
// struct FNamedFloatThreshold
// {
// 	GENERATED_BODY()
// public:
// 	/* What the threshold is. */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float Threshold;

// 	/* What the threshold represents. */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	FText Description;

// 	FNamedFloatThreshold()
// 	{
// 		Threshold = 0.0f;
// 		Description = FText::GetEmpty();
// 	}

// 	FNamedFloatThreshold(float Threshold, const FText& Description) : Threshold(Threshold), Description(Description) {}
// };

/**
 * Panel which shows details about a character.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UCharacterPanel : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterPortrait;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterNameText;

	UPROPERTY(meta = (BindWidget))
	UWrapBox* ModifiersBox;

	UPROPERTY(meta = (BindWidget))
	USkillIcon* AbilityIcon;

	UPROPERTY(meta = (BindWidget))
	USkillIcon* AttackIcon;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Never used internally, but links a character panel to a character. */
	int32 CharacterPanelID;

protected:
	/* Blueprint class of an icon representing a modifier. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler", meta = (BlueprintBaseOnly))
	TSubclassOf<UModifierIcon> ModifierIconClass;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Constructs a character panel with basic information.
	 * @param CharacterIcon Texture representing character icon
	 * @param CharacterName Name of the character.
	 * @param CharacterBaseStats Used to configure base (non modified) stats of this character.
	 * @param AttackAsset Asset used to configure attack icon widget.
	 * @param AbilityAsset Asset used to configure ability icon widget.
	 */
	void BuildCharacterPanel(
		UTexture2D* CharacterIcon, 
		const FText& CharacterName, 
		const FCharacterBaseStats& CharacterBaseStats,
		const UAutobattlerAttack* AttackAsset, 
		const UAutobattlerAbility* AbilityAsset
	);

/////////////////////////////////////////////////////////////////////////////////
//// API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Adds a modifier to the modifiers box.
	 * @param IconImage Icon representing modifier
	 * @param ModifierDescription User-friendly description of modifier.
	 * @param Duration If this modifier has a duration, pass via this parameter.
	 */
	void AddModifier(UTexture2D* IconImage, const FText& ModifierDescription, float Duration = -1.0f);

	/**
	 * Updates a stat value if an AutobattlerStatWidget representing that stat exists in the list of
	 * child widgets.
	 * @param StatTypeToModify What stat we are modifying.
	 * @param NewValue The numeric or text value to set for the stat.
	 * @param IsBase Whether we are modifying the base (actual) or updating its "modified" value.
	 */
	void UpdateStat(EAutobattlerStatType StatTypeToModify, float NewValue, bool IsBase);
	void UpdateStat(EAutobattlerDiscreteType StatTypeToModify, const FText& NewValue, bool IsBase);
};
