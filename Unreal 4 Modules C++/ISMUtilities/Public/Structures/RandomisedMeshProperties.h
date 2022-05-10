// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RandomisedMeshProperties.generated.h"

UENUM(BlueprintType)
enum class EBoundsType : uint8
{
	Box UMETA(DisplayName = "Box"),
	Cylinder UMETA(DisplayName = "Cylinder")
};

USTRUCT(BlueprintType)
struct FRandomisedMeshProperties
{
	GENERATED_BODY()
public:
	/* Actual mesh to be used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMesh* Mesh;

	/* Picks a randomised rotation between MinRotation and MaxRotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator MinRotation;

	/* Picks a randomised rotation between MinRotation and MaxRotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator MaxRotation;

	/* Picks a randomised scale between MinScale and MaxScale. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MinScale = FVector::OneVector;

	/* Picks a randomised scale between MinScale and MaxScale. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MaxScale = FVector::OneVector;

	/* This vector is added to the relative location. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AlignmentAdjust;

	/* Whether or not alignment should be multiplied by scale. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bScaleAlignmentAdjust = true;

	/* The bounding shape of the mesh. This is used when adjusting meshes to prevent overlaps. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBoundsType BoundsType;

	/* The approximate X/Y/Z dimensions encompassing the mesh. Only relevant if BoundsType = Box. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="BoundsType==EBoundsType::Box"))
	FVector BoundingBoxExtent;

	/* Radius of the sphere encompassing the mesh. Only relevant if BoundsType = Sphere. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="BoundsType==EBoundsType::Sphere"))
	float BoundingSphereRadius;

	/* Gets a rotator between MinRotation and MaxRotation. */
	FRotator RandomisedRotator() const
	{
		return FRotator(FMath::RandRange(MinRotation.Pitch, MaxRotation.Pitch), FMath::RandRange(MinRotation.Yaw, MaxRotation.Yaw), FMath::RandRange(MinRotation.Roll, MaxRotation.Roll));
	}

	/* Gets a scale between MinScale and MaxScale. This randomised scale is used by AlignmentAdjustScaled() */
	FVector RandomisedScale()
	{
		RandomisedScaleInternal = FVector(FMath::RandRange(MinScale.X, MaxScale.X), FMath::RandRange(MinScale.Y, MaxScale.Y), FMath::RandRange(MinScale.Z, MaxScale.Z));
		return RandomisedScaleInternal;
	}

	/* Scales alignment adjust by the current randomised scale. */
	FVector AlignmentAdjustScaled() const
	{
		return bScaleAlignmentAdjust ? AlignmentAdjust * RandomisedScaleInternal : AlignmentAdjust;
	}

private:
	FVector RandomisedScaleInternal = FVector::OneVector;
};

USTRUCT(BlueprintType)
struct FRange
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max;

	FRange()
	{
		Min = 0.0f;
		Max = 0.0f;
	}

	FRange(float NewMin, float NewMax)
	{
		Min = NewMin;
		Max = NewMax;
	}

	float GenerateRandomNumberInRange(const FRandomStream& RandomStream = FRandomStream(), bool bUseStream = false) const
	{
		if (Min > Max) return Min;
		return bUseStream ? Min + (Max - Min) * RandomStream.FRand() : FMath::RandRange(Min, Max);
	}
};

USTRUCT(BlueprintType)
struct FRandomScaleProperties
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRandomScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bUseRandomScale"))
	bool bUseStream;

	/* If true, uses scale in range. If false, generates completely random scale (WARNING: This may result in huge scales). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bUseRandomScale"))
	bool bUseRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRandomScale && bUseRange"))
	FRange XRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRandomScale && bUseRange"))
	FRange YRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRandomScale && bUseRange"))
	FRange ZRange;

	FRandomScaleProperties()
	{
		bUseRandomScale = true;
		bUseStream = true;
		bUseRange = true;
		XRange = FRange(0.1f, 1.0f);
		YRange = FRange(0.1f, 1.0f);
		ZRange = FRange(0.1f, 1.0f);
	}

	FVector GenerateRandomScale(const FRandomStream& RandomStream = FRandomStream()) const
	{
		if (!bUseRandomScale) return FVector::OneVector;

		if (bUseRandomScale && !bUseStream && !bUseRange)
		{
			FVector RScale;
			RScale.X = FMath::FRand();
			RScale.Y = FMath::FRand();
			RScale.Z = FMath::FRand();
			return RScale;
		}

		if (bUseRandomScale && bUseStream && !bUseRange)
		{
			FVector RScale;
			RScale.X = RandomStream.FRand();
			RScale.Y = RandomStream.FRand();
			RScale.Z = RandomStream.FRand();
			return RScale;
		}

		if (bUseRandomScale && !bUseStream && bUseRange)
		{
			FVector RScale;
			RScale.X = XRange.GenerateRandomNumberInRange();
			RScale.Y = YRange.GenerateRandomNumberInRange();
			RScale.Z = ZRange.GenerateRandomNumberInRange();
			return RScale;
		}

		if (bUseRandomScale && bUseStream && bUseRange)
		{
			FVector RScale;
			RScale.X = XRange.GenerateRandomNumberInRange(RandomStream, true);
			RScale.Y = YRange.GenerateRandomNumberInRange(RandomStream, true);
			RScale.Z = ZRange.GenerateRandomNumberInRange(RandomStream, true);
			return RScale;	
		}

		return FVector::OneVector;
	}
};

USTRUCT(BlueprintType)
struct FRandomRotationProperties
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRandomRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bUseRandomRotation"))
	bool bUseStream;

	/* If true, uses rotator in range. If false, generates completely random rotator. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bUseRandomRotation"))
	bool bUseRange;

	/* When generating a completely random rotator, should roll be used? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRoll && bUseRandomRotation && bUseRange"))
	FRange RollRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRandomRotation && bUseRange"))
	FRange PitchRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseRandomRotation && bUseRange"))
	FRange YawRange;

	FRandomRotationProperties()
	{
		bUseRandomRotation = true;
		bUseStream = true;
		bUseRange = false;
		bUseRoll = false;
	}

	/* Random stream only relevant if bUseStream specified. */
	FRotator GenerateRandomRotator(const FRandomStream& RandomStream = FRandomStream()) const
	{
		if (!bUseRandomRotation) return FRotator::ZeroRotator;
		if (bUseRandomRotation && !bUseStream && !bUseRange)
		{
			FRotator RRot;
			RRot.Pitch = FMath::FRand() * 360.0f;
			RRot.Yaw = FMath::FRand() * 360.0f;
			RRot.Roll = bUseRoll ? FMath::FRand() * 360.0f : 0.0f;
			return RRot;
		}

		if (bUseRandomRotation && bUseStream && !bUseRange)
		{
			FRotator RRot;
			RRot.Pitch = RandomStream.FRand() * 360.0f;
			RRot.Yaw = RandomStream.FRand() * 360.0f;
			RRot.Roll = bUseRoll ? RandomStream.FRand() * 360.0f : 0.0f;
			return RRot;
		}

		if (bUseRandomRotation && !bUseStream && bUseRange)
		{
			FRotator RRot;
			RRot.Pitch = PitchRange.GenerateRandomNumberInRange();
			RRot.Yaw = YawRange.GenerateRandomNumberInRange();
			RRot.Roll = bUseRoll ? RollRange.GenerateRandomNumberInRange() : 0.0f;
			return RRot;
		}

		if (bUseRandomRotation && bUseStream && bUseRange)
		{
			FRotator RRot;
			RRot.Pitch = PitchRange.GenerateRandomNumberInRange(RandomStream, true);
			RRot.Yaw = YawRange.GenerateRandomNumberInRange(RandomStream, true);
			RRot.Roll = bUseRoll ? RollRange.GenerateRandomNumberInRange(RandomStream, true) : 0.0f;
			return RRot;
		}

		return FRotator::ZeroRotator;
	}
};

USTRUCT(BlueprintType)
struct FRandomMeshTemplate
{
	GENERATED_BODY()

public:
	/* Actual mesh to be used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMesh* Mesh;

	/* Properties specifying random rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRandomRotationProperties RandomRotationProperties;

	/* Properties specifying random scale. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRandomScaleProperties RandomScaleProperties;
};
