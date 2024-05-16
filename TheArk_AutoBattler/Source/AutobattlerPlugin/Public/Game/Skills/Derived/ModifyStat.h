// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Skills/SkillImplementation.h"
#include "Types/AutobattlerEnums.h"
#include "ModifyStat.generated.h"

class UTexture2D;

/**
 * Skill which modifies a stat on the target character.
 * Modifiers can be positive or negative.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UModifyStat : public USkillImplementation
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// SKILL IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Which (numeric) stat should be modified. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modify Stat")
	EAutobattlerStatType StatToModify;

	/* Scale of the modifier, which depends on what type of stat should be modified.
	E.g. If StatToModify is ActionSpeed, this value is how much ActionSpeed is increased/decreased by. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modify Stat")
	float ModifierScale = 1.0f;

	/* Whether this modification is permanent or whether it expires after a duration. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modify Stat")
	bool IsPermanent = false;

	/* Only relevant if IsPermanent = false. How long before the modifier wears off. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modify Stat")
	float ExpiryTime = 5.0f;

	/* If true, a separate modifier will be added to the modifier box. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modify Stat")
	bool UseModifier;

	/* The icon which will be used for the modifier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modify Stat", meta = (EditCondition="UseModifier"))
	UTexture2D* ModifierIcon;

	/* The text which will be used for the modifier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modify Stat", meta = (EditCondition="UseModifier"))
	FText ModifierDescription;

	/**
	 * Deals damage to each of the targets.
	 * @param SkillOwner Unused.
	 * @param SkillTargetingMode This should be actor, otherwise does nothing.
	 * @param Target Target which modifier should be applied to.
	 * @param Location Unused. 
	 */
	virtual void ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation) override;
};
