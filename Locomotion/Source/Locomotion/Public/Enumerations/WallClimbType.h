// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "WallClimbType.generated.h"

UENUM(BlueprintType)
enum class EWallClimbType : uint8
{
	High UMETA(DisplayName = "High"),
	Low  UMETA(DisplayName = "Low"),
	Fall UMETA(DisplayName = "Fall")
};
