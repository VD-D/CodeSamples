// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "EquipType.generated.h"

UENUM(BlueprintType)
enum class EEquipType : uint8
{
	EquipRifle     UMETA(DisplayName = "Equip"),
	UnequipRifle   UMETA(DisplayName = "Unequip")
};
