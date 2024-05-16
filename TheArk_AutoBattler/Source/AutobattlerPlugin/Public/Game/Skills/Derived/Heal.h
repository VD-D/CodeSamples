// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Skills/SkillImplementation.h"
#include "Heal.generated.h"

/**
 * Skill implementation for healing between min and max.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UHeal : public USkillImplementation
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// SKILL IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Min/Max heal this skill deals. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal")
	FRandomRange MinMaxHeal;

	/**
	 * Heals the target.
	 * @param SkillOwner Unused.
	 * @param SkillTargetingMode This should be actor.
	 * @param Target Target to deal damage to.
	 * @param Location Unused. 
	 */
	virtual void ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation) override;
};
