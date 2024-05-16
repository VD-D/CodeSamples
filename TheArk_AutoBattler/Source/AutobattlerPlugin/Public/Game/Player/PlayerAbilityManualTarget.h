// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Player/PlayerAbilityGetTarget.h"
#include "PlayerAbilityManualTarget.generated.h"

/**
 * Used for any player ability which requires manual targeting.
 */
UCLASS(Abstract)
class AUTOBATTLERPLUGIN_API UPlayerAbilityManualTarget : public UPlayerAbilityGetTarget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Overridable logic for whether a target is valid.
	 * @param MouseHitResult Hit result under cursor.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler")
	bool IsTargetValid(UAutobattlerControllerComponent* SkillOwner, const FHitResult& MouseHitResult) const;
	virtual bool IsTargetValid_Implementation(UAutobattlerControllerComponent* SkillOwner, const FHitResult& MouseHitResult) const;

	/**
	 * Set the last hit result from player controller for this implementation.
	 * @param MouseHitResult Hit result to set.
	 */
	void SetLastHitResult(const FHitResult& MouseHitResult) { LastHitResult = MouseHitResult; }

	/**
	 * @return THe last hit result from player controller upon attemping an interact.
	 */
	FHitResult GetLastHitResult() const { return LastHitResult; }

private:
	/* Last hit result cache. */
	FHitResult LastHitResult;
};
