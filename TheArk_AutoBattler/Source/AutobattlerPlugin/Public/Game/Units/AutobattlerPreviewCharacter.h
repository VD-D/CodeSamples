// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerPreviewCharacter.generated.h"

class AAutobattlerGrid;
class UBoxComponent;
class UMaterialInterface;
class USkeletalMesh;
class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCanBePlacedReason, EEntity, WhoOwns, EPlaceReason, PlacementReason);

/**
 * Character which serves as a preview an actual character while it is being placed on the
 * autobattler grid.
 */
UCLASS(Blueprintable)
class AUTOBATTLERPLUGIN_API AAutobattlerPreviewCharacter : public AActor
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// DELEGATES
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Called on both server and client. Notifies the reason why the character cannot (or can) be placed. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FCanBePlacedReason CanBePlacedReason;

/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* CharacterMesh;

	/* A bounding box which checks if it is overlapping with any other autobattler characters when the preview character moves. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CharacterDeploymentBounds;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Material to use on primitive components when placement location is valid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	UMaterialInterface* CanPlaceMaterial;

	/* Material to use on primitive components when placement location is not valid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	UMaterialInterface* CannotPlaceMaterial;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Whether or not this character can be placed (i.e. is on a valid grid location). */
	UPROPERTY(ReplicatedUsing = "OnRep_CanBePlaced")
	bool CanBePlaced;

	/* Whether this character should be visible at all. */
	UPROPERTY(ReplicatedUsing = "OnRep_ShouldBeVisible")
	bool ShouldBeVisible;

	/* This character's internal ID. */
	int32 PreviewCharacterID;

	/* Budget cost of this character. */
	UPROPERTY(Replicated)
	int32 PreviewCharacterCost;

	/* The final placement location, accounting for offset. */
	UPROPERTY(ReplicatedUsing = "OnRep_FinalPlacementLocation")
	FVector FinalPlacementLocation;

	/* How the deployment character should be offset from the center of the grid tile. */
	FVector PlacementOffset;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	AAutobattlerPreviewCharacter();

	/**
	 * Builds this preview character from the given parameters.
	 * @param NewMesh Skeletal mesh to assign to the preview character.
	 * @param PreviewAnimation Animation to use when previewing.
	 * @param ID Relevant only for the server. Internal ID of this character.
	 * @param BudgetCost How expensive this preview character is w.r.t. the budget (used for updating pending budget).
	 */
	void ConfigurePreviewCharacter(const FVector& DeploymentOffset, int32 ID, int32 BudgetCost);

protected:
	/**
	 * Used to bind delegates.
	 */
	virtual void BeginPlay() override;

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Getter for whether or not this preview character can be currently placed.
	 * @return True if it can be placed.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetCanBePlaced() const { return CanBePlaced; }

	/**
	 * Getter for this character's ID.
	 * @return This character's ID.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	int32 GetPreviewCharacterID() const { return PreviewCharacterID; }

/////////////////////////////////////////////////////////////////////////////////
//// NETWORKING
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Network scaffolding.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/**
	 * Executes default implementation of placement state.
	 */
	UFUNCTION()
	void OnRep_CanBePlaced();

	/**
	 * Executes default implementation of placement state.
	 */
	UFUNCTION()
	void OnRep_ShouldBeVisible();

	/**
	 * Moves character to final placement location.
	 */
	UFUNCTION()
	void OnRep_FinalPlacementLocation();

/////////////////////////////////////////////////////////////////////////////////
//// GAME
/////////////////////////////////////////////////////////////////////////////////
protected:
	/**
	 * Called whenever CanBePlaced is changed. Default functionality changes materials on all primitive components
	 * to Can or Cannot be placed materials.
	 * @param NewCanBePlaced Whether placement is possible.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler")
	void OnCanBePlaceStateChanged(bool NewCanBePlaced);
	virtual void OnCanBePlaceStateChanged_Implementation(bool NewCanBePlaced);

private:
	/**
	 * Used to update preview character's state based on where they can be placed.
	 * @param WhoChanged Unused.
	 * @param ValidLocation Whether moved to location was valid.
	 * @param NewLocation Location of mouse to grid.
	 * @param NewGridIndex Unused.
	 */
	UFUNCTION()
	void OnGridLocationChanged(EEntity WhoChanged, bool ValidLocation, const FVector& NewLocation, const FIntPair& NewGridIndex);

	/**
	 * Shows pending budget cost for a given entity.
	 * @param WhoOwns For whom to show pending budget cost for.
	 * @param NewShow Whether to hide or show pending budget cost.
	 * @param Cost Actual budget cost.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void ShowPendingBudgetCost(EEntity WhoOwns, bool NewShow, int32 Cost);

	/**
	 * Broadcast delegate which informs why or why not this preview character can be placed.
	 * @param WhoOwns Owner of this preview character.
	 * @param PlacementReason What the current placement status is.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void BroadcastCannotBePlaced(EEntity WhoOwns, EPlaceReason PlacementReason);

	/**
	 * Utility function for changing visibility of all scene components.
	 * @param NewVisible True to make visible, false to hide.
	 */
	void ChangeSceneComponentVisibility(bool NewVisible);

	/**
	 * Utility function for changing materials to can/cannot be placed for all primitive components.
	 * @param CanPlace True for can place, false for cannot place.
	 */
	void UpdatePrimitiveComponentMaterials(bool CanPlace);
};
