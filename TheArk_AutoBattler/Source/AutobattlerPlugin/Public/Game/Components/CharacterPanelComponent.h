// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/AutobattlerStructs.h"
#include "CharacterPanelComponent.generated.h"

class UCharacterPanel;

/**
 * Any actor which has this component can display a character panel (e.g. deployment or autobattler characters).
 * 
 * The CharacterPanelComponent is a component responsible for processing information about the state of an autobattler character
 * to a CharacterPanelWidget. This is a widget showing that information to the player.
 * These panels are never created locally, since information may change at runtime, and so the CharacterPanel widget needs to 
 * be updated at runtime (on both client and server).
 */
UCLASS(ClassGroup=(Autobattler), meta=(BlueprintSpawnableComponent))
class AUTOBATTLERPLUGIN_API UCharacterPanelComponent : public UActorComponent
{
	GENERATED_BODY()

//////////////////////////////////////////////////////////////////////////////
//// INTERNAL
//////////////////////////////////////////////////////////////////////////////
private:	
	/* The constructed character panel. */
	UPROPERTY()
	UCharacterPanel* CharacterPanel;

	/* Whether showing/hiding character panel is locked. */
	bool IsLocked;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	UCharacterPanelComponent();

	/**
	 * Creates a new character panel based on the definition.
	 * @param CharacterPanelClass Class of the character panel.
	 * @param NewDefinition New definition to create character panel from.
	 * @param ID ID of character who owns this panel.
	 */
	UFUNCTION(BlueprintCallable, Category = "Character Panel")
	void BuildFromCharacterDefinition(TSubclassOf<UCharacterPanel> CharacterPanelClass, const FAutobattlerCharacterDefinition& NewDefinition);

private:
	/**
	 * Internal, replicated version for building the character panel.
	 * @param CharacterPanelClass Class of the character panel.
	 * @param NewIcon New character icon.
	 * @param NewName New character name.
	 * @param NewBaseStats New base stats for character.
	 * @param NewAttackDefinition New attack definition for character.
	 * @param NewAbilityDefinition New ability definition for character.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void BuildFromDefinitionInternal(TSubclassOf<UCharacterPanel> CharacterPanelClass, UTexture2D* NewIcon, const FText& NewName, const FCharacterBaseStats& NewBaseStats, const UAutobattlerAttack* NewAttackDefinition, const UAutobattlerAbility* NewAbilityDefinition);

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Getter for whether showing/hiding character panel is locked.
	 * @return Whether showing/hiding character panel is locked.
	 */
	UFUNCTION(BlueprintPure, Category = "Character Panel")
	bool GetIsLocked() const { return IsLocked; }

/////////////////////////////////////////////////////////////////////////////////
//// CHARACTER PANEL API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Static helper function to hide character panel currently being shown.
	 */
	static void HideActiveCharacterPanel(const UObject* WorldContextObject);

	/**
	 * Shows/hides the character panel. Constructs widget and updates character panel from listing.
	 * @param NewShow True to show, false to hide.
	 * @return Whether we successfully hid/showed character panel.
	 */
	bool ShowCharacterPanel(bool NewShow);

	/**
	 * If NewLock = true, shows character panel and prevents it from disappearing until unlocked.
	 * @param NewLock True prevents this character panel from ever disappearing.
	 */
	void LockCharacterPanel(bool NewLock);

	/**
	 * Analagous to AddModifier in CharacterPanel. Adds a modifier to the modifiers box.
	 * @param IconImage Icon representing modifier
	 * @param ModifierDescription User-friendly description of modifier.
	 * @param Duration If this modifier has a duration, pass via this parameter.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void AddModifier(UTexture2D* IconImage, const FText& ModifierDescription, float Duration = -1.0f);

	/**
	 * Analagous to UpdateStat in CharacterPanel.
	 * Updates a stat value if an AutobattlerStatWidget representing that stat exists in the list of
	 * child widgets.
	 * @param StatTypeToModify What stat we are modifying.
	 * @param NewValue The numeric or text value to set for the stat.
	 * @param IsBase Whether we are modifying the base (actual) or updating its "modified" value.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void UpdateStatNumeric(EAutobattlerStatType StatTypeToModify, float NewValue, bool IsBase);

	/**
	 * Analagous to UpdateStat in CharacterPanel.
	 * Updates a stat value if an AutobattlerStatWidget representing that stat exists in the list of
	 * child widgets.
	 * @param StatTypeToModify What stat we are modifying.
	 * @param NewValue The numeric or text value to set for the stat.
	 * @param IsBase Whether we are modifying the base (actual) or updating its "modified" value.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void UpdateStatDiscrete(EAutobattlerDiscreteType StatTypeToModify, const FText& NewValue, bool IsBase);
};
