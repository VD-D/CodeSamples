// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathPoints.generated.h"

UCLASS()
class INFILTRATION_API APathPoints : public AActor
{
	GENERATED_BODY()
	
public:	
	/**
	 * Constructor
	 */
	APathPoints();

	/* Points defined by this path. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path Points", meta = (MakeEditWidget))
	TArray<FVector> Points;

	/**
	 * Gets the world location of a value from Points. Returns ZeroVector if the Index is invalid.
	 * @param Index - Index of Points. Must be a valid index.
	 */
	UFUNCTION(BlueprintCallable, Category = "Path Points")
	FVector MakeWorldTransformFromPoint(int32 Index);
};
