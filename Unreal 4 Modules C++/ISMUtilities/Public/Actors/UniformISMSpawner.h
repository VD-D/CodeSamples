// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structures/RandomisedMeshProperties.h"
#include "UniformISMSpawner.generated.h"

UCLASS()
class ISMUTILITIES_API AUniformISMSpawner : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* Root component of the actor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneRoot;

	/* Set this to true to rebuild it in the editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ISM Grid Spawner")
	bool bRebuild;

	/* If true, the instanced mesh will be laid out vertically rather than horizontally (i.e. transformed around the Z axis rather than the Y axis). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ISM Grid Spawner")
	bool bConstructVertically;

	/* When spawning meshes, a random one will be used from this list with the given properties. To use only 1, just have this array contain 1 element. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ISM Grid Spawner")
	TArray<FRandomisedMeshProperties> Meshes;

	/* Rows/Columns the instanced mesh component. E.g. a 2 by 3 setup would spawn 6 (2 * 3) instanced, in two rows and three columns. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ISM Grid Spawner")
	FIntPoint Extent;

	/* X/Y Dimensions of the actual Mesh, specified above. This must be correct, otherwise the instanced static mesh will be created incorrectly. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ISM Grid Spawner")
	FVector2D XYDimensions;

	/* Any indicies stored here will be ignored when building the mesh. This allows for cutting "holes" in the mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ISM Grid Spawner")
	TArray<FIntPoint> IgnoreIndicies;

	/* What type of collision to use for the instanced static mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ISM Grid Spawner|Collision")
	TEnumAsByte<ECollisionEnabled::Type> InstancedMeshCollisionEnabled;

	/* What type of object the instanced static mesh is. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ISM Grid Spawner|Collision")
	TEnumAsByte<ECollisionChannel> CollisionObjectType;

	/* Desired Collision responses for the instanced static mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ISM Grid Spawner|Collision")
	FCollisionResponseContainer CollisionResponses;

private:
	/* Array for easy reference of spawned ISM components. */
	UPROPERTY()
	TMap<int32, class UInstancedStaticMeshComponent*> ISMComps;

/* --- FUNCTIONS --- */
public:	
	/**
	 * Constructor.
	 */
	AUniformISMSpawner();

	/**
	 * Function which builds the instanced static meshes.
	 * Automatically called if Rebuild is set to true.
	 */
	UFUNCTION(BlueprintCallable, Category = "ISM Grid Spawner")
	void BuildInstancedMesh();

protected:
	/**
	 * Construction script which is called if Rebuild is true.
	 * @param Transform Unused.
	 */
	virtual void OnConstruction(const FTransform& Transform) override;
};
