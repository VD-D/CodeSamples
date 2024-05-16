// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Enumerations/MovementDirection.h"
#include "PivotParameters.generated.h"

USTRUCT(BlueprintType)
struct FPivotParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float PivotDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	EMovementDirection CompletedMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float CompletedStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	EMovementDirection InterruptedMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float InterruptedStartTime;
};
