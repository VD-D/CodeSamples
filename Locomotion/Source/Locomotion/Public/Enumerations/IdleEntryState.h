// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "IdleEntryState.generated.h"

UENUM(BlueprintType)
enum class EIdleEntryState : uint8
{
	Neutral   UMETA(DisplayName = "Neutral"),
	Aiming    UMETA(DisplayName = "Aiming"),
	Crouching UMETA(DisplayName = "Crouching")
};
