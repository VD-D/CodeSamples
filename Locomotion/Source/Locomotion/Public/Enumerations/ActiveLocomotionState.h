// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "ActiveLocomotionState.generated.h"

UENUM(BlueprintType)
enum class EActiveLocomotionState : uint8
{
	None      UMETA(DisplayName = "None"),
	NotMoving UMETA(DisplayName = "NotMoving"),
	Moving    UMETA(DisplayName = "Moving"),
	Pivot     UMETA(DisplayName = "Pivot"),
	Stopping  UMETA(DisplayName = "Stopping")
};
