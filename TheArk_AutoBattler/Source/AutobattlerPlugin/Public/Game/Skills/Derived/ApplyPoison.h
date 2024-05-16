// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Skills/SkillImplementation.h"
#include "ApplyPoison.generated.h"

/**
 * Skill implementation for applying poison.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UApplyPoison : public USkillImplementation
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// SKILL IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Min/Max poison strength. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deal Damage")
	FRandomRange MinMaxPoison;

	/**
	 * Deals damage to each of the targets.
	 * @param SkillOwner Unused.
	 * @param SkillTargetingMode This should be actor.
	 * @param Target Target to deal damage to.
	 * @param Location Unused. 
	 */
	virtual void ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation) override;
};
