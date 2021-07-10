// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "WallClimbTraceSettings.generated.h"

USTRUCT(BlueprintType)
struct FWallClimbTraceSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float MaxLedgeHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float MinLedgeHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float ReachDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float ForwardTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float DownTraceRadius;
};
