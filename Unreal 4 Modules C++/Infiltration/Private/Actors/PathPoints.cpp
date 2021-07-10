// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PathPoints.h"

APathPoints::APathPoints()
{
	/* Tick is not required. */
	PrimaryActorTick.bCanEverTick = false;
}

FVector APathPoints::MakeWorldTransformFromPoint(int32 Index)
{
	if (Points.IsValidIndex(Index)) return GetActorTransform().TransformPosition(Points[Index]);
	return FVector::ZeroVector;
}