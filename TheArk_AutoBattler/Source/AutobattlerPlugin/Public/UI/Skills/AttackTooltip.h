// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttackTooltip.generated.h"

class UAutobattlerAttack;
class UImage;
class URichTextBlock;
class UTextBlock;

/**
 * Tooltip displaying information about an attack, such as its range, effect,
 * speed, who it targets, etc.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAttackTooltip : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* AbilityIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttackName;

	UPROPERTY(meta = (BindWidget))
	URichTextBlock* AttackDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttackRange;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttackActionSpeed;

	UPROPERTY(meta = (BindWidget))
	URichTextBlock* AttackTargetDescription;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Builds this tooltip.
	 * @param AbilityAsset Asset from which to source data from.
	 */
	void BuildTooltipFromAsset(const UAutobattlerAttack* AttackAsset);

protected:
	/**
	 * Called after the tooltip is built.
	 * @param AbilityAsset Ability asset this tooltip was built from. 
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler")
	void OnAbilityTooltipBuilt(const UAutobattlerAttack* AttackAsset);
	virtual void OnAbilityTooltipBuilt_Implementation(const UAutobattlerAttack* AttackAsset);
};
