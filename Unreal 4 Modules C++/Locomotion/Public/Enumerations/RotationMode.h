// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "RotationMode.generated.h"

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	VelocityDirection UMETA(DisplayName = "VelocityDirection"),
	TargetDirection   UMETA(DisplayName = "TargetDirection")
};
