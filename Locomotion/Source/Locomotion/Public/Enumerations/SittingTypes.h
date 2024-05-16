// Copyright Robert Uszynski 2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "SittingTypes.generated.h"

UENUM(BlueprintType)
enum class ESittingTypes : uint8
{
	ArmRest UMETA(DisplayName = "ArmRest"),
	Backwards UMETA(DisplayName = "Backwards"),
	Bound UMETA(DisplayName = "Bound"),
	FemaleLegCrossOne UMETA(DisplayName = "FemaleLegCrossOne"),
	FemaleLegCrossTwo UMETA(DisplayName = "FemaleLegCrossTwo"),
	FootUp UMETA(DisplayName = "FootUp"),
	HandsBehindHead UMETA(DisplayName = "HandsBehindHead"),
	HandsOnLap UMETA(DisplayName = "HandsOnLap"),
	LeanOne UMETA(DisplayName = "LeanOne"),
	LeanTwo UMETA(DisplayName = "LeanTwo"),
	LegsCross UMETA(DisplayName = "LegsCross"),
	LegsSpread UMETA(DisplayName = "LegsSpread"),
	RestHands UMETA(DisplayName = "RestHands"),
	Slouch UMETA(DisplayName = "Slouch"),
	Thinker UMETA(DisplayName = "Thinker")
};
