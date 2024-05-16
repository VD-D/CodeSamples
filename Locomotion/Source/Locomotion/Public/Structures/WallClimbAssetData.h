// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "WallClimbAssetData.generated.h"

USTRUCT(BlueprintType)
struct FWallClimbAssetData
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
	float LowHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float LowPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float LowStartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float HighHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float HighPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float HighStartPosition;
};
