// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/VisibilityScalingInterface.h"
#include "LineOfSightVisualiser.generated.h"

UENUM(BlueprintType)
enum class EVisualisation : uint8
{
	Peripheral UMETA(DisplayName = "Peripheral"),
	Full       UMETA(DisplayName = "Full")
};

UCLASS( ClassGroup=(LineOfSightVisualisation), meta=(BlueprintSpawnableComponent) )
class LINEOFSIGHTVISUALISATION_API ULineOfSightVisualiser : public UActorComponent, public IVisibilityScalingInterface
{
	GENERATED_BODY()

/* VARIABLES */
public:
	/* --- CONFIGURABLE --- */
	/* The number of times the mesh is sub-divided before edge-detection calculations are made.
	Higher values increase precision at the cost of performance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	int32 SubdivisionCount;

	/* The number of times edge-detection calculations are made per-frame.
	Higher values increase accuracy at significant cost in performance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	int32 IterationsCount;

	/* The procedural mesh's resolution. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	float MeshResolution;

	/* The minimal distance between edges to warrant additional traces. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	float MinEdgeIdentifierDistance;

	/* The rate at which the arc radius is interpolate from current to target if it is changed
	at runtime. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	float VisionInterpRate;

	/* The name of the procedural mesh component used by this actor. If this is invalid,
	this component will do nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	FString VisualisationMeshName;

	/* Whether or not debug messages should be enabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	bool bEnableDebugMessages;

	/* Whether this should use peripheral or full params from ILineOfSightParameterInterface. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	EVisualisation VisualisationType;

	/* The trace channel to use when performing line traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	/* The material which the procedural mesh will use. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualisation Configuration")
	class UMaterialInterface* VisulisationMaterial;

protected:
	/* Reference to the Visualisation Mesh. */
	class UProceduralMeshComponent* VisualisationMesh;

private:
	/* If InitialiseMesh has been called. */
	bool bHasBeenInitialised;

	/* Whether the procedural mesh should be force-updated; this is usually a result
	of tick being disabled. */
	bool bDoesMeshNeedToBeUpdated;

	/* Whether or not an obstacle was detected in current update cycle. */
	bool bWasObstacleDetectedInCurrentCycle;

	/* The number of line traces being performed at a given frame. Used for profiling and debug. */
	int32 LineTraceCount;

	/* The number of points required to be created given mesh resolution and vision angle. Is higher
	at greater vision angles and mesh resolutions. */
	int32 MinimalLoSPoints;

	/* The vision angle normalised to 0 - 360 degrees. */
	float NormalizedVisionAngle;

	/* Angular interval between traces. */
	float MeshDivisionAngle;

	/* The angle in mesh subdivisions. */
	float EdgeSubdivision;

	/* Adjusted mesh resolution to ensure all subdivisions are of equal resolution. */
	float CorrectedMeshResolution;

	/* Current vision radius. */
	float CurrentVisionRadius;

	/* Maximal vision radius. */
	float MaxVisionRadius;

	/* Target vision radius. If different from CurrentVisionRadius, will be FInterp'd to. */
	float TargetVisionRadius;

	/* Parameters used for traces */
	FCollisionQueryParams Params;

/* --- FUNCTIONS --- */
public:
	/** 
	 * Constructor. 
	 */
	ULineOfSightVisualiser();

	/** 
	 * Updates the mesh per-tick.
	 * @param DeltaTime - The time between this and the last frame. Unused.
	 * @param TickType - What type of tick this is (e.g. time only or viewport only). Unused.
	 * @param ThisTickFunction - Reference to this tick function. Unused.
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Disables the visualisation if it is currently enabled.
	 */
	void DisableVisualisation();

	/** 
	 * Implementation of ScaleVisibility from IVisibilityScalingInterface. Adjusts TargetArcRadius.
	 * @param ScaleAmount - Passed as Scale to ScaleVision.
	 */
	virtual void ScaleVisibility_Implementation(float& Scale) override;

protected:
	/** 
	 * Begin rendering the mesh.
	 */
	virtual void BeginPlay() override;

private:
	/** 
	 * Initalise the procedural mesh to its initial state. Does nothing if the owner of this component does not
	 * implement ILineOfSightParamtersInterface or MeshResolution <= 0.0f.
	 */
	void InitaliseMesh();

	/**
	 * Enable visualisation. Enables tick if it has been disabled and begins the visualastion process.
	 * @warning Only works if bHasBeenInitialised = true. Call Super::BeginPlay().
	 */
	void EnableVisualisation();

	/**
	 * Updates the visualisation per frame.
	 * @param DeltaTime - World delta seconds.
	 * @warning Can cause SERIOUS performance issues with overly-high values of IterationsCount and/or SubdivisionCount.
	 */
	void UpdateVisualisation(float DeltaTime);

	/**
	 * Creates a singular triangular section of the procedural mesh.
	 * @param SectionIndex - The "i-th" triangle of the mesh.
	 * @param Verticies - An array of vectors whose length MUST be a multiple of 3.
	 */
	void CreateMeshSection(const int32 SectionIndex, const TArray<FVector> Vertices);

	/** 
	 * Updates a singular triangular section of the procedural mesh.
	 * @param SectionIndex - The "i-th" triangle of the mesh. If this section does not exist, the update does nothing.
	 * @param Vertices - An array of vectors whose length MUST be a multiple of 3. Updates the section of the mesh.
	 */
	void UpdateMeshSection(const int32 SectionIndex, const TArray<FVector> Vertices);

	/**
	 * Creates an int32 TArray of triangle indicies from a FVector TArray.
	 * @param Verticies - An array of FVector representing a triangle. Length must be a multiple of 3.
	 * @return An array of triangle vertex indicies.
	 */
	TArray<int32> CreateTriangleDataFromVertices(const TArray<FVector> Vertices);

	/**
	 * Creates a FVector TArray of vertices from trace points.
	 * @param TracePoints - The trace points from which to build vertex data from.
	 * @param ShouldFocusOnDefaultVerticies - Whether these vertices are being built as an additional section or not.
	 * @return A FVector TArray which contains the new vertex data.
	 */
	TArray<FVector> CreateVertexDataFromPoints(const TArray<FVector> Points, const bool ShouldFocusOnDefaultVerticies);

	/**
	 * Generates points to draw Line of Sight to based on the current spatial parameters.
	 * @param bWasObstactleDetected - (mutable) Whether or not an obstacle was detected.
	 * @param Points - (mutable) The list of points to draw LoS to.
	 */
	void GeneratePoints(bool &bWasObstactleDetected, TArray<FVector> &Points);

	/** 
	 * Performs a single line trace from the VisualisationMesh's location to VisionRadius
	 * (retrieved from ILineOfSightParameterInterface) and returns the hit result.
	 * @param TraceAngle - The desired angle between the owner's location and the end point.
	 * @return The FHitResult of the trace.
	 */
	FHitResult SingleTrace(const float TraceAngle);

	/** 
	 * Wrapper function which returns Hit.ImpactPoint if Hit.bBlockingHit otherwise returns Hit.TraceEnd.
	 * @param Hit - The FHitResult to test.
	 * @return Either Hit.ImpactPoint or Hit.Trace head, depending on whether there was a blocking hit or not.
	 */
	FVector BlockingTraceEnd(FHitResult Hit);

	/**
	 * DEPRECATED 
	 */
	bool IsMeshUpdateRequired(const bool ObstacleDetectedInPrevCycle, const bool ObstacleDetectedInCurrentCycle);

	/** 
	 * Tests for the presence of an edge between two hit results.
	 * @param StartingHit - The first hit result.
	 * @param EndingHit - The second hit result.
	 * @return Whether or not an edge is present between the two hit results.
	 */
	bool PerformEdgeTest(const FHitResult StartingHit, const FHitResult EndingHit);

	/**
	 * Performs a subdivision between the previous and current point and finds (to an approximation)
	 * the locations of the edges between them.
	 * @param StartingAngle - The angle between the StartingHit and the desired next point.
	 * @param StartingHit - The starting hit result.
	 * @return A TArray<FVector> of additional points representing edge locations.
	 */
	TArray<FVector> ConstructEdgeFixApproximation(const float StartingAngle, const FHitResult StartingHit);

	/**
	 * Calculates (to greater accuracy) edge points within a subdivided sector, specified by the first 5 parameters.
	 * This function is iterated SubdivisionCount number of times.
	 * @param StartingHit - The starting hit result.
	 * @param StartingAngle - The angle between the owner's forward axis and the start of the trace.
	 * @param EndingAngle - The angle between the owner's forward axis and the end of the trace.
	 * @param StartingPoint - The end of the trace at StartingAngle.
	 * @param EndingPoint - The end of the trace at EndingAngle.
	 * @param StartUpdated - True if the starting point was updated.
	 * @param UpdatedStartPoint - Updated if the starting point was updated, otherwise == StartingPoint.
	 * @param EndUpdated - True if the end point was updated.
	 * @param UpdatedEndPoint - Updated if the ending point was updated, otherwise == EndingPoint.
	 */
	void RecalculateEdgePoints(const FHitResult StartingHit, const float StartingAngle, const float EndingAngle, const FVector StartingPoint, const FVector EndingPoint, bool &StartUpdated, FVector &UpdatedStartPoint, bool &EndUpdated, FVector &UpdatedEndPoint);

	/**
	 * Performs A % B, returns 0 if B == 0
	 * @param A - Left hand side of the modulo operator. 
	 * @param B - Right hand side of the modulo operator.
	 * @return A % B or 0 if B == 0
	 */
	float FModChecked(float A, float B);
};
