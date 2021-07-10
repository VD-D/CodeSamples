// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LineOfSightParameterInterface.generated.h"

USTRUCT(BlueprintType)
struct FLineOfSightParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float VisionRadiusPeripheral;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float VisionAnglePeripheral;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float VisionRadiusFull;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float VisionAngleFull;
};

UINTERFACE(MinimalAPI)
class ULineOfSightParameterInterface : public UInterface
{
	GENERATED_BODY()
};

/* Interface for passing line of sight parameters between. */
class LINEOFSIGHTVISUALISATION_API ILineOfSightParameterInterface
{
	GENERATED_BODY()

public:
	/** 
	 * Retrieves a FLineOfSightParameters struct which determines how the LineOfSightVisualiser
	 * should behave.
	 * @param LineOfSightParameters - A FLineOfSightParameters struct with set parameters.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LineOfSightParameterInterface")
	void GetLineOfSightParameters(FLineOfSightParameters& LineOfSightParameters);
};
