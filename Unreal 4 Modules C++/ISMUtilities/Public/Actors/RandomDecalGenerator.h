// Copyright Robert Zygmunt Uszynski 2021-2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structures/RandomisedMeshProperties.h"
#include "RandomDecalGenerator.generated.h"

UENUM(BlueprintType)
enum class EDecalSpawnAxis : uint8
{
	PositiveX UMETA(DisplayName = "PositiveX"),
	NegativeX UMETA(DisplayName = "NegativeX"),
	PositiveY UMETA(DisplayName = "PositiveY"),
	NegativeY UMETA(DisplayName = "NegativeY"),
	PositiveZ UMETA(DisplayName = "PositiveZ"),
	NegativeZ UMETA(DisplayName = "NegativeZ")
};

/* Utility actor for randomly spawning decals over an area. */
UCLASS()
class ISMUTILITIES_API ARandomDecalGenerator : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneRoot;

	/* Indicates direction in which decals will be spawned. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Refresh")
	bool bRefresh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration")
	FVector Bounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration")
	EDecalSpawnAxis DecalSpawnAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration", meta = (ClampMin="0", ClampMax="89"))
	float MaxSpawnAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration")
	bool bAlignToNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration")
	bool bUseRandomSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration")
	int32 Seed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration")
	int32 NumberDecalsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration")
	TArray<class UMaterialInterface*> DecalTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Trace")
	TArray<class AActor*> ActorsToIgnoreInTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Trace")
	bool bTraceComplex;

	/* If false, will use default X decal size of 128. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Size")
	bool bRandomiseXSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Size", meta = (EditCondition="bRandomiseXSize"))
	FRange XSizeRange;

	/* If false, will use default X decal size of 256. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Size")
	bool bRandomiseYSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Size", meta = (EditCondition="bRandomiseYSize"))
	FRange YSizeRange;

	/* If false, will use default X decal size of 256. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Size")
	bool bRandomiseZSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Size", meta = (EditCondition="bRandomiseZSize"))
	FRange ZSizeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Rotation")
	bool bRandomiseRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Rotation", meta = (EditCondition="bRandomiseRoll"))
	FRange RollRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Rotation")
	bool bRandomisePitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Rotation", meta = (EditCondition="bRandomisePitch"))
	FRange PitchRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Rotation")
	bool bRandomiseYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Decal Configuration|Rotation", meta = (EditCondition="bRandomiseYaw"))
	FRange YawRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Random Decal Configuration")
	FColor BoxColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Random Decal Configuration")
	float LineThickness;

private:
	UPROPERTY()
	TArray<class UDecalComponent*> DecalComponents;

	FRandomStream StreamSeed;

public:	
	/**
	 * Constructor.
	 */
	ARandomDecalGenerator();

	/**
	 * Spawns decals from settings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Random Decal Generator")
	void SpawnDecals();

protected:
	/**
	 * Sets up arrow.
	 * @param Transform Unused.
	 */
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	/**
	 * Utility to get base decal rotation from rotation axis.
	 * @param DecalSpawnAxis Spawn Axis.
	 * @return Base rotation
	 */
	FRotator GetDecalRotationFromDecalSpawnAxis(EDecalSpawnAxis NewDecalSpawnAxis);

	/**
	 * Utility to get align decal to ground and normal if so desired.
	 * @param ComponentToModify Decal component to align.
	 * @param CollisionQueryParams Query params to use.
	 */
	void AlignDecalComponent(class UDecalComponent* ComponentToModify, const FCollisionQueryParams& CollisionQueryParams);

	/**
	 * Helper function for getting a random float from stream in a range.
	 * @param Min Min range.
	 * @param Max Max range.
	 * @return Random Number in range from stream.
	 */
	float RandomPointInRangeFromStreamSeed(float Min, float Max);

	/**
	 * Gets a random point in the Bounds, accounting for Seed and actor's current location (i.e. is world-space random location).
	 * @return Random point in bounding box.
	 */
	FVector RandomPointInBounds();

	void GetTraceStartEndLocations(const FVector& RandomPoint, FVector& StartTraceLocation, FVector& EndTraceLocation);
};
