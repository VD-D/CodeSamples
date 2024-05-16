// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "SittingType.generated.h"

UENUM(BlueprintType)
enum class ESittingType : uint8
{
	Slouched    UMETA(DisplayName = "Slouched"),
	Sleeping    UMETA(DisplayName = "Sleeping"),
	LeanForward UMETA(DisplayName = "LeanForward"),
	Shady       UMETA(DisplayName = "Shady"),
	Flirtatious UMETA(DisplayName = "Flirtatious")
};
