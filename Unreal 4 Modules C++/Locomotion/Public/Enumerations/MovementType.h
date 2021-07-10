// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "MovementType.generated.h"

UENUM(BlueprintType)
enum class EMovementType : uint8
{
	None       UMETA(DisplayName = "None"),
	Grounded   UMETA(DisplayName = "Grounded"),
	Falling    UMETA(DisplayName = "Falling"),
	Ragdoll    UMETA(DisplayName = "Ragdoll"),
	WallWaking UMETA(DisplayName = "WallWalking")
};
