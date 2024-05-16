// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Player/PlayerAbilityManualTarget.h"
#include "PlayerAbility_SingleCharacter.generated.h"

/**
 * Manually targets a single character appropriate to the target type.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UPlayerAbility_SingleCharacter : public UPlayerAbilityManualTarget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Returns true if our target is an autobattler character within the list of allowed entities to target.
	 * @param SkillOwner Component who owns the skill.
	 * @param MouseHitResult Hit result from player.
	 */
	virtual bool IsTargetValid_Implementation(UAutobattlerControllerComponent* SkillOwner, const FHitResult& MouseHitResult) const override;
};
