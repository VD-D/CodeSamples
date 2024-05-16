// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "MovementDirection.generated.h"

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward  UMETA(DisplayName = "Forward"),
	Backward UMETA(DisplayName = "Backward")
};
