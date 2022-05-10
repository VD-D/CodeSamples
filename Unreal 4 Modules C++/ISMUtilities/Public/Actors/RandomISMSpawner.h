// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structures/RandomisedMeshProperties.h"
#include "RandomISMSpawner.generated.h"

UENUM(BlueprintType)
enum class ESpawnShape : uint8
{
	Box UMETA(DisplayName = "Box"),
	Cylinder UMETA(DisplayName = "Cylinder")
};

UCLASS()
class ISMUTILITIES_API ARandomISMSpawner : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* Root component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneComponent;

	/* Set to true to regenerate meshes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM Generate")
	bool bRegenerate;

	/* Whether to randomly distribute meshes over a box shape or cylinder shape. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings")
	EBoundsType SpawnShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings", meta = (EditCondition="SpawnShape==EBoundsType::Box"))
	FVector BoxExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings", meta = (EditCondition="SpawnShape==EBoundsType::Cylinder"))
	float CylinderRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings", meta = (EditCondition="SpawnShape==EBoundsType::Cylinder"))
	float CylinderExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings")
	TEnumAsByte<ECollisionChannel> SpawnOnCollisionChannel;

	/* Whether or not meshes should be aligned to normal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings")
	bool bAlignToNormal;

	/* If false, it will use a true "random" distribution. Seed uses pseudo-random distribution. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings")
	bool bUseRandomSeed;

	/* Seed to use is UseRandomSeed = true. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings", meta = (EditCondition="bUseRandomSeed"))
	int32 Seed;

	/* Spread between meshes. The greater this number, the greater the distance between meshes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings", meta = (ClampMin="1"))
	float Spread;

	/* The maximum angle meshes can be spawned on. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings", meta = (ClampMin="0", ClampMax="89"))
	float MaxSpawnAngle;

	/* Number of meshes to spawn. This may not be the actual number if there is no space. Meshes are randomly picked from MeshTemplates. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings")
	int32 SpawnCount;

	/* Actors which HISMs can be spawned on. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings")
	TArray<class AActor*> SpawnOnActors;

	/* Array of templates for meshes to spawn. Meshes can be given a random scale and rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM General Settings")
	TArray<FRandomMeshTemplate> MeshTemplates;

	/* Maximal number of iterations to attempt to adjust meshes so they have space. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Random ISM General Settings")
	int32 MaxAdjustmentIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM Mesh Settings")
	bool bCastShadows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM Mesh Settings")
	int32 HISMMaxCullDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM Mesh Settings")
	float MaxDrawDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM Mesh Settings")
	TEnumAsByte<ECollisionEnabled::Type> CollisionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Random ISM Mesh Settings", meta = (EditCondition="CollisionType!=ECollisionEnabled::Type::NoCollision"))
	FCollisionResponseContainer CollisionResponses;

	/* Whether or not to display debug shapes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random ISM Debug Settings")
	bool bDrawDebugShapes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Random ISM Debug Settings")
	float DebugShapesLineThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Random ISM Debug Settings")
	FLinearColor DebugShapesColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Random ISM Debug Settings")
	float CutShapesLineThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Random ISM Debug Settings")
	FLinearColor CutShapesColor;

private:
	FRandomStream StreamSeed;
	int32 SpawnedCount;
	int32 NumberIterationsExpired;

	/* Requires UPROPERTY() specifier for it to not be garbage collected upon editor shutdown.*/
	UPROPERTY()
	TArray<class UHierarchicalInstancedStaticMeshComponent*> HISMs;

public:	
	/**
	 * Constructor.
	 */
	ARandomISMSpawner();

	/**
	 * Main driving function. Can be safely called in construction script.
	 */
	UFUNCTION(BlueprintCallable, Category = "Random ISM")
	void SpawnRandomisedMeshes();

protected:
	/**
	 * Override of construction script to spawn meshes and debug shapes.
	 * @param Transform Unused.
	 */
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	/**
	 * Makes a cylinder shape centered at the world location of this object.
	 * @param NewCylinderRadius Radius of the cylinder.
	 * @param StartLocation Start location of the cylinder.
	 * @param EndLocation End location of the cylinder.
	 */
	void MakeCylinderShape(float& NewCylinderRadius, FVector& StartLocation, FVector& EndLocation);

	/**
	 * Gets a random point in the spawn box, accounting for using seed.
	 */
	FVector InteralGetRandomPointInBox();

	/**
	 * Gets a random point in the spawn sphere, accounting for using seed.
	 */
	FVector InternalGetRandomPointInSphere();

	/**
	 * Recursive function which attempts to find a random point which is not colliding with anything other than desired actors and
	 * is not colliding with other HISM components.
	 */
	bool RandomPointChecked(FVector& FinalRandomLocation, FVector& ImpactNormal, const FRotator& Rotation, const FVector& Scale, class UHierarchicalInstancedStaticMeshComponent* HISMToUse);

	/**
	 * Helper function for getting a random float from stream in a range.
	 * @param Min Min range.
	 * @param Max Max range.
	 * @return Random Number in range from stream.
	 */
	float RandomPointInRangeFromStreamSeed(float Min, float Max);

	/**
	 * Makes a square shape centered at the world location of this object. 
	 */
	// void MakeSquareShape(FVector& Center, FVector& Extent);
};
