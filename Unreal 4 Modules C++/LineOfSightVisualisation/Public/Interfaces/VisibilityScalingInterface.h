// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VisibilityScalingInterface.generated.h"

UINTERFACE(MinimalAPI)
class UVisibilityScalingInterface : public UInterface
{
	GENERATED_BODY()
};

/*  */
class LINEOFSIGHTVISUALISATION_API IVisibilityScalingInterface
{
	GENERATED_BODY()

public:
	/** 
	 * Triggered when visibility extent should be scaled.
	 * @param ScaleAmount - (mutable) The amount to scale by.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VisibilityScaling")
	void ScaleVisibility(float& ScaleAmount);
};
