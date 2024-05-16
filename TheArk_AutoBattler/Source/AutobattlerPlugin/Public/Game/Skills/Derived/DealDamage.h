// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Skills/SkillImplementation.h"
#include "Types/AutobattlerStructs.h"
#include "DealDamage.generated.h"

/**
 * Skill implementation for dealing damage between Min and Max.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UDealDamage : public USkillImplementation
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// SKILL IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Min/Max damage this skill deals. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deal Damage")
	FRandomRange MinMaxDamage;

	/* Whether or not the damage dealt is affected by resistances. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deal Damage")
	bool AffectedByResistance = true;

	/* Whether or not the damage dealt can be a critical strike. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deal Damage")
	bool CanBeCritical = true;

	/**
	 * Deals damage to each of the targets.
	 * @param SkillOwner Unused.
	 * @param SkillTargetingMode This should be actor.
	 * @param Target Target to deal damage to.
	 * @param Location Unused. 
	 */
	virtual void ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation) override;
};
