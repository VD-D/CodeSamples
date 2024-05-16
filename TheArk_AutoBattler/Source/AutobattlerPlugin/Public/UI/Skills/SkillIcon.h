// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillIcon.generated.h"

class UAbilityTooltip;
class UAttackTooltip;
class UAutobattlerSkill;
class UImage;
class UTextBlock;
class UWidget;

/**
 * Icon representing a skill (either an ability or attack).
 * Automatically infers whether the tooltip should show information based on the skill type passed.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API USkillIcon : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* SkillIconImage;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* SkillNameText;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Class from which to create ability tooltip from. */
	UPROPERTY(EditAnywhere, Category = "Autobattler", meta = (BlueprintBaseOnly))
	TSubclassOf<UAbilityTooltip> AbilityTooltipClass;

	/* Class from which to create attack tooltip from. */
	UPROPERTY(EditAnywhere, Category = "Autobattler", meta = (BlueprintBaseOnly))
	TSubclassOf<UAttackTooltip> AttackTooltipClass;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Skill to create icon from. */
	UPROPERTY()
	const UAutobattlerSkill* CurrentSkill;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Sets the data asset used to create tooltip.
	 * @param SkillAsset Asset to create tooltip from.
	 */
	void SetSkillDataAsset(const UAutobattlerSkill* SkillAsset);

private:
	/**
	 * Creates an ability or attack tooltip depending on configured skill.
	 * @return Attack tooltip if skill is an attack, Ability tooltip if skill is an ability.
	 */
	UFUNCTION()
	UWidget* GetSkillWidget();
};
