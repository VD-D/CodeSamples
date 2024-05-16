// Copyright Juggler Games 2022 - 2024
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/AutobattlerStructs.h"
#include "DurationSkillComponent.generated.h"

class AAutobattlerCharacter;
class UAutobattlerSkill;

/**
 * Component responsible for skills which occur over a duration.
 */
UCLASS(ClassGroup=("Autobattler"))
class AUTOBATTLERPLUGIN_API UDurationSkillComponent : public UActorComponent
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* How many times the skill has been repeated. */
	int32 NumIntervalsExpired;

	/* Timer handle for the duration. */
	FTimerHandle DurationHandle;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	UDurationSkillComponent();

	/**
	 * Configures a skill which should last a duration.
	 * @param Target Target to initialise this skill on.
	 * @param SkillAsset Asset which skill list will be executed on.
	 * @param SkillOwner Character who executed this skill.
	 * @param TargetingProperties Initial targeting properties.
	 */
	static void ConfigureTimedSkill(AAutobattlerCharacter* Target, const UAutobattlerSkill* SkillAsset, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties);

protected:
	/**
	 * Internal use only. Actually begins timer and execution of skill on target.
	 * @param SkillAsset Asset which skill list will be executed on.
	 * @param SkillOwner Character who executed this skill.
	 * @param TargetingProperties Initial targeting properties.
	 */
	void ConfigureTimedSkillInternal(const UAutobattlerSkill* SkillAsset, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties);

	/**
	 * Cleans up the timer should the owner of this duration component be destroyed.
	 */
	virtual void BeginDestroy() override;

/////////////////////////////////////////////////////////////////////////////////
//// TIMER
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Reactivates this skill when the interval expires.
	 */
	UFUNCTION()
	void ReactivateSkill(const UAutobattlerSkill* SkillAsset, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties);
};
