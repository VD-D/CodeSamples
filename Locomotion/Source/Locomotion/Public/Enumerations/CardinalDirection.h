// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "CardinalDirection.generated.h"

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	North UMETA(DisplayName = "North"),
	South UMETA(DisplayName = "South"),
	East  UMETA(DisplayName = "East"),
	West  UMETA(DisplayName = "West")
};
