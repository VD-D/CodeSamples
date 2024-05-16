// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Player/PlayerAbilityAutoTarget.h"
#include "PlayerAbility_TargetAllAuto.generated.h"

/**
 * Targeting logic for any player ability which targets all characters of type WhatToTarget.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UPlayerAbility_TargetAllAuto : public UPlayerAbilityAutoTarget
{
	GENERATED_BODY()
	
};
