// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "LocomotionActivity.generated.h"

UENUM(BlueprintType)
enum class ELocomotionActivity : uint8
{
	Base      UMETA(DisplayName = "Base"),
	CellPhone UMETA(DisplayName = "CellPhone"),
	Weapon    UMETA(DisplayName = "Weapon"),
	Sitting   UMETA(DisplayName = "Sitting")
};
