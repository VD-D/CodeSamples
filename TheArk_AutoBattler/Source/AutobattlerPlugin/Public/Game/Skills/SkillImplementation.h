// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/Skills/ExecuteSkill.h"
#include "Types/AutobattlerEnums.h"
#include "SkillImplementation.generated.h"

class AAutobattlerCharacter;
class UGetTarget;
class UParticleSystem;
class UStaticMesh;

/**
 * This class implements what a skill should do once it triggers.
 * Derived classes should expose variables which affect how ExecuteSkill() works (e.g. How much damage the skill deals).
 */
UCLASS(Blueprintable, Abstract, DefaultToInstanced, EditInlineNew)
class AUTOBATTLERPLUGIN_API USkillImplementation : public UObject
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// SKILL IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Function which should be overriden by child classes to execute the skill's effect.
	 * @param SkillOwner The character who executed this skill.
	 * @param SkillTargetingMode Who this skill wants to target.
	 * @param Target Target for this skill.
	 * @param TargetLocation The location this skill targets, usually relevant if SkillTargetingMode == Location.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Skill Implementation")
	void ExecuteSkill(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation);
	virtual void ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation);

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* What the collision type of this skill is (mainly used for abilities with AOE). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	ESkillCollisionType SkillCollisionType = ESkillCollisionType::Single;

	/* Relevant for who is targeted *only when SkillCollisionType is not single*; whether they are an enemy, ally or self (or combination). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	EAbilityFilterType SecondaryTargetFilter = EAbilityFilterType::Self;

	/* What the collision radius is if the attack collision type is sphere. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	float SkillCollisionRadius = 32.0f;

	/* What the collision box extent is if the attack collision type is box. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	FVector SkillBoxExtent = FVector(100.0f);
};
