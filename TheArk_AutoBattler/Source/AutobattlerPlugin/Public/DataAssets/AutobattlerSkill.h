// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Game/Components/DurationSkillComponent.h"
#include "Game/Skills/SkillImplementation.h"
#include "AutobattlerSkill.generated.h"

class UAnimSequence;
class UGetTarget;
class USkillImplementation;

/**
 * Data asset defining a character's skill.
 */
UCLASS(Abstract)
class AUTOBATTLERPLUGIN_API UAutobattlerSkill : public UDataAsset
{
	GENERATED_BODY()
public:
	/* User-friendly skill name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText SkillName;

	/* User-friendly skill description. Rich text supported. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText SkillDescription;

	/* Icon representing the skill. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* SkillIcon;

	/* If we need a specific ExecuteSkill class for executing the attack (may apply to some unique attacks
	which modify the way how a hit/projectile/effect works), change it here. Otherwise, leave as default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AExecuteSkill> ExecuteSkillClass = AExecuteSkill::StaticClass();

	/* How this skill behaves; each separate skill is triggered in order when the skill is "cast", possibly with different visual effects. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<USkillImplementation*> SkillEffects;

	/* Whether this skill repeats (i.e. last over a duration). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration")
	bool HasDuration;

	/* Whether the skill activates initially; false means it will activate after the first interval. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration", meta = (EditCondition="HasDuration"))
	bool ActivatesImmediately;

	/* How long between activations. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration", meta = (EditCondition="HasDuration", ClampMin="0.1"))
	float Interval = 1.0f;

	/* How many times the skill repeats. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration", meta = (EditCondition="HasDuration", ClampMin="1"))
	int32 NumRepetitions = 3;

	/* Can leave at default, unless you wish to use a custom duration skill component e.g. to activate special effects.
	Remember to call parent implementation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration", meta = (EditCondition="HasDuration"))
	TSubclassOf<UDurationSkillComponent> DurationSkillComponentClass = UDurationSkillComponent::StaticClass();
	
	/* Animations to use for this action; one is picked at random. Note that you will need to add the
	TriggerSkill anim notify on each animation in order for the skill to be triggered. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UAnimSequence*> SkillAnimations;

	/* In reality, the playback speed of SkillAnimation, lower values result in the trigger happening slower. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ActionSpeed = 1.0f;

	/* How far this skill reaches (this is irrelevant if WhoIsAffected is Self). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	float Range = 120.0f;

	/* Relevant for who is targeted; whether they are an enemy, ally or self (or combination). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	EAbilityFilterType PrimaryTargetFilter = EAbilityFilterType::Self;

	/* User-friendly description of possible targets (e.g. Nearest Enemy). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TargetDescription;

	/* Class which returns who/where has to be targeted in order for the skill to go off. See UGetTarget::GetTargetForSkill. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	TSubclassOf<UGetTarget> GetTargetImplementationClass;

	/* What happens when the skill triggers.
	Instant = Effect is applied instantly.
	Projectile = Spawn projectile to hit target.
	Effect = Spawn particle effect at target's location. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	ESkillTriggerEnd OnSkillTriggerEnd = ESkillTriggerEnd::Instant;

	/* Optional skeletal mesh socket name to spawn skill at if it is a projectile. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation", meta = (EditCondition = "OnSkillTriggerEnd==ESkillTriggerEnd::Projectile"))
	FName ProjectileSocketSpawnName = FName("None");

	/* How long before the skill effect will "timeout" and disappear. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation", meta = (EditCondition = "OnSkillTriggerEnd==ESkillTriggerEnd::Effect"))
	float EffectTimeout = 1.0f;

	/* Particle to use as projectile if OnSkillTriggerEnd is of type projectile, or the effect if OnSkillTriggerEnd is of type effect. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation", meta = (EditCondition = "OnSkillTriggerEnd!=ESkillTriggerEnd::Instant"))
	UParticleSystem* SkillParticleEffect;

	/* Mesh to use as projectile if OnSkillTriggerEnd is of type projectile, or the effect if OnSkillTriggerEnd is of type effect. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation", meta = (EditCondition = "OnSkillTriggerEnd!=ESkillTriggerEnd::Instant"))
	UStaticMesh* SkillMesh;
};
