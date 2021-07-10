// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "WallClimbParameters.generated.h"

USTRUCT(BlueprintType)
struct FWallClimbParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	class UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	class UCurveVector* VectorAdjustCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FVector StartingOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float StartPosition;
};
