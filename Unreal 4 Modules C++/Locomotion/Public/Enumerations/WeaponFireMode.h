// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "WeaponFireMode.generated.h"

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	None       UMETA(DisplayName = "None"),
	Continuous UMETA(DisplayName = "Continuous"),
	Single     UMETA(DisplayName = "Single"),
	Burst      UMETA(DisplayName = "Burst")
};
