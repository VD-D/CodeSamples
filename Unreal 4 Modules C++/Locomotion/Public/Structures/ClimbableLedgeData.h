// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "ClimbableLedgeData.generated.h"

USTRUCT(BlueprintType)
struct FClimbableLedgeData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FTransform TargetTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	class UPrimitiveComponent* HitComponent;
};
