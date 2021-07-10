// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Stance.generated.h"

UENUM(BlueprintType)
enum class EStance : uint8
{
	Standing  UMETA(DisplayName = "Standing"),
	Crouching UMETA(DisplayName = "Crouching")
};
