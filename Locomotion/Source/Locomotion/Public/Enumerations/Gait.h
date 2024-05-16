// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Gait.generated.h"

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walking   UMETA(DisplayName = "Walking"),
	Running   UMETA(DisplayName = "Running"),
	Sprinting UMETA(DisplayName = "Sprinting")
};
