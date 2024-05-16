// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerGrid.generated.h"

class UDecalComponent;
class UMaterialInterface;
class USceneComponent;

USTRUCT()
struct FIndexInfo
{
	GENERATED_BODY()
public:
	/* Decal component at this grid index. */
	UPROPERTY()
	UDecalComponent* DecalComponent;

	/* Actual grid index. */
	UPROPERTY()
	FIntPair GridLocation;

	FIndexInfo()
	{
		DecalComponent = nullptr;
		GridLocation = FIntPair();
	}

	FIndexInfo(UDecalComponent* NewDecalComponent, const FIntPair& NewGridLocation)
	{
		DecalComponent = NewDecalComponent;
		GridLocation = NewGridLocation;
	}
};

/**
 * Class which generates the grid to be used in the autobattler game. 
 */
UCLASS(Blueprintable)
class AUTOBATTLERPLUGIN_API AAutobattlerGrid : public AActor
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Root offset of the grid. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* GridRoot;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Material to use for the decal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Grid")
	UMaterialInterface* DefaultDecalMaterial;

	/* How many tiles on x-axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Grid", meta = (ClampMin = "1"))
	int32 GridXSize;

	/* How many tiles on y-axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Grid", meta = (ClampMin = "1"))
	int32 GridYSize;

	/* How far above/below the grid an actor can be to be considered "on it". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Grid")
	float MaxZExtent;

	/* X/Y size of the grid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Grid")
	float XYSize;

	/* How much larger or smaller the decal material should be for the grid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Grid")
	float DecalShrinkFactor;

	/* Whether the grid is hidden in game by default. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Grid")
	bool HiddenByDefault;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Info about various properties present on a grid index. */
	UPROPERTY(Replicated)
	TArray<FIndexInfo> GridIndexInfo;

	/* Locally changed variable as to whether the grid is currently hidden. */
	bool IsCurrentlyHidden;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	AAutobattlerGrid();

	/**
	 * Generates grid visualisation from parameters.
	 */
	UFUNCTION(CallInEditor, Category = "Autobattler Grid")
	void BuildGridInEditor();

protected:
	/**
	 * Builds the grid if BuildGridOnBeginPlay is enabled. 
	 */
	virtual void BeginPlay() override;

private:
	/**
	 * Function which actually builds the grid.
	 */
	void BuildGrid();

	/**
	 * Binds delegates from manager to show/hide grid under various circumstances
	 * (see individual functions).
	 */
	void BindManagerDelegates();

/////////////////////////////////////////////////////////////////////////////////
//// UTILITY
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Getter whether grid is currently hidden (locally).
	 * @return Whether grid is currently hidden (locally).
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Grid")
	bool GetIsCurrentlyHidden() const { return IsCurrentlyHidden; }

	/**
	 * Gets grid width.
	 * @return Grid width.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Grid")
	float GetGridWidth() const { return GridYSize * XYSize; }

	/**
	 * Gets grid height.
	 * @return Grid height.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Grid")
	float GetGridHeight() const { return GridXSize * XYSize; }

	/**
	 * Getter for max z extent.
	 * @return Grid max z extent (how far above/below an actor can be to be considered "on" grid).
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Grid")
	float GetGridMaxZExtent() const { return MaxZExtent; }

	/**
	 * Getter for X/Y size of each tile.
	 * @return X/Y size of each tile.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Grid")
	float GetGridXYSize() const { return XYSize; }

	/**
	 * Test to see if a grid index is valid.
	 * @param TestGridIndex Grid index to test.
	 * @return True if index is valid, false if not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Grid")
	bool IsValidGridIndex(const FIntPair& TestGridIndex) const;

	/**
	 * Gets the grid index at a given location. Valid means XY position within MaxZExtent above or below
	 * the grid.
	 * @param Location Location to test.
	 * @param OutGridIndex (OUT) The location of the grid index if valid, (-1, -1) otherwise.
	 * @return True if a valid location is found. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Grid")
	bool LocationToGridIndex(const FVector& Location, FIntPair& OutGridIndex) const;

	/**
	 * Gets the location at the given grid index (centered), as well as the hit result
	 * of tracing from MaxZExtent
	 * @param Location (OUT) World location of point on grid.
	 * @param TestGridIndex Grid index we want to find location of.
	 * @return True if the Grid index was found and the location was valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Grid")
	bool GridIndexToLocation(const FIntPair& TestGridIndex, FVector& Location, FHitResult& HitResult, TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility, bool TraceComplex = false) const;

/////////////////////////////////////////////////////////////////////////////////
//// UTILITY
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Toggles visibility for entire grid.
	 * @param ShouldBeVisible True = visible, false = invisible.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Grid")
	void ToggleGridVisibility(bool ShouldBeVisible);

	/**
	 * Shows only desired indicies, hides all others.
	 * @param IndiciesToShow Which grid indicies we want to show.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Grid")
	void ShowOnlyIndicies(const TArray<FIntPair>& IndiciesToShow);

/////////////////////////////////////////////////////////////////////////////////
//// NETWORKING
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Network scaffolding.
	 * @param OutLifetimeProps Unused.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
