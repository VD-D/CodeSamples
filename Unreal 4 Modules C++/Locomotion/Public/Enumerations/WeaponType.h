// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "WeaponType.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None            UMETA(DisplayName = "None"),
	IronsightsRifle UMETA(DisplayName = "IronsightsRifle"),
	HipRifle        UMETA(DisplayName = "HipRifle")
};
