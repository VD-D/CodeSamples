// Fill out your copyright notice in the Description page of Project Settings.

/* Class header. */
#include "Game/Player/PlayerAbilityManualTarget.h"

bool UPlayerAbilityManualTarget::IsTargetValid_Implementation(UAutobattlerControllerComponent* SkillOwner, const FHitResult& MouseHitResult) const
{
    // By default returns true.
    return true;
}
