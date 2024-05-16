// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Player/PlayerAbilityGetTarget.h"
#include "PlayerAbilityAutoTarget.generated.h"

/**
 * Used for any player ability which automatically targets.
 */
UCLASS(Abstract)
class AUTOBATTLERPLUGIN_API UPlayerAbilityAutoTarget : public UPlayerAbilityGetTarget
{
	GENERATED_BODY()
	
};
