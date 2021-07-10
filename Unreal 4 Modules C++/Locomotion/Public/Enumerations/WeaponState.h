// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "WeaponState.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	None                UMETA(DisplayName = "None"),
	RaisingToHip        UMETA(DisplayName = "RaisingToHip"),
	RaisingToIronsights UMETA(DisplayName = "RaisingToIronsights"),
	AtHip               UMETA(DisplayName = "AtHip"),
	AtIronsights        UMETA(DisplayName = "AtIronsights"),
	FiringHip           UMETA(DisplayName = "FiringHip"),
	FiringIronsights    UMETA(DisplayName = "FiringIronsights"),
	ReloadingHip        UMETA(DisplayName = "ReloadingHip"),
	ReloadingIronsights UMETA(DisplayName = "ReloadingIronsights"),
	LoweringFromHip        UMETA(DisplayName = "LoweringFromHip"),
	LoweringFromIronsights UMETA(DisplayName = "LoweringFromIronsights")
};
