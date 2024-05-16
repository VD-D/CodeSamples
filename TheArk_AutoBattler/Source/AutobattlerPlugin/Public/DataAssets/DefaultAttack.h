// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Game/Skills/ExecuteSkill.h"
#include "Types/AutobattlerStructs.h"
#include "DefaultAttack.generated.h"

class UAnimSequence;
class UGetTarget;

/**
 * Data asset defining a base attack.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UDefaultAttack : public UDataAsset
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// DATA
/////////////////////////////////////////////////////////////////////////////////
public:
	/* If we need a specific ExecuteSkill class for executing the attack (may apply to some unique attacks
	which are not from the standard hit/projectile/effect template), change it here. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AExecuteSkill> ExecuteSkillClass = AExecuteSkill::StaticClass();

	/* Animation to use for this attack. Note that you will need to add the
	TriggerSkill anim notify to this animation in order for the attack to be triggered. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequence* AttackAnimation;

	/* How far this attack reaches (in reality sphere radius). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 50;

	/* How far this attack reaches. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Range = 1;

	/* In reality, the playback speed of AttackAnimation, lower values result in the trigger happening slower. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ActionSpeed = 1.0f;

	/* What happens when the skill triggers.
	Instant = Effect is applied instantly (no further effects).
	Projectile = Spawn projectile to hit target.
	Effect = Spawn effect at target's location. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillTriggerEnd OnSkillTriggerEnd;

	/* Optional skeletal mesh socket name to spawn skill at if it is a projectile. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "OnSkillTriggerEnd==ESkillTriggerEnd::Projectile"))
	FName ProjectileSocketSpawnName;

	/* How long before the skill effect will "timeout" and disappear. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "OnSkillTriggerEnd==ESkillTriggerEnd::Effect"))
	float EffectTimeout;

	/* What the collision type of this skill is (mainly used for attacks which splash). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillCollisionType AttackCollisionType;

	/* What the collision radius is if the attack collision type is sphere. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "AttackCollisionType==ESkillCollisionType::Sphere"))
	float AttackCollisionRadius;

	/* What the collision box extent is if the attack collision type is box. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "AttackCollisionType==ESkillCollisionType::Box"))
	FVector AttackBoxExtent;

	/* Particle to use as projectile if OnSkillTriggerEnd is of type projectile, or the effect if OnSkillTriggerEnd is of type effect. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* SkillParticleEffect;

	/* Mesh to use as projectile if OnSkillTriggerEnd is of type projectile, or the effect if OnSkillTriggerEnd is of type effect. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* SkillMesh;

	/* Class which returns the intended target of this attack. See UGetTarget::GetTarget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGetTarget> GetTargetImplementationClass;

	/* Min/Max damage dealt by the attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRandomRange MinMaxDamage;
};
