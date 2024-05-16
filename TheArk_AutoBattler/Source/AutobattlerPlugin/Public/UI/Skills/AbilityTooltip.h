// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityTooltip.generated.h"

class UAutobattlerAbility;
class UImage;
class URichTextBlock;
class UTextBlock;

/**
 * Tooltip displaying information about an ability, such as its cast range, effect,
 * speed, who it targets, etc.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAbilityTooltip : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* AbilityIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillName;

	UPROPERTY(meta = (BindWidget))
	URichTextBlock* SkillDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillRange;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillActionSpeed;

	UPROPERTY(meta = (BindWidget))
	URichTextBlock* SkillTargetDescription;

	/* This is the cooldown if uses cooldown, otherwise this is the charge counter. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillCounter;

	/* Only shown if the our trigger type is by gaining charges. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GainChargesCondition;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Builds this tooltip.
	 * @param AbilityAsset Asset from which to source data from.
	 */
	void BuildTooltipFromAsset(const UAutobattlerAbility* AbilityAsset);

protected:
	/**
	 * Called after the tooltip is built.
	 * @param AbilityAsset Ability asset this tooltip was built from. 
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler")
	void OnAbilityTooltipBuilt(const UAutobattlerAbility* AbilityAsset);
	virtual void OnAbilityTooltipBuilt_Implementation(const UAutobattlerAbility* AbilityAsset);
};
