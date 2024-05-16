// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/AutobattlerStructs.h"
#include "RedeployPreview.generated.h"

class UDecalComponent;
class USceneComponent;
class UStaticMesh;
class UMaterialInterface;

/**
 * Actor which visualises the redeployment of an already deployed character.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API ARedeployPreview : public AActor
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Root component for offseting. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* ActorRoot;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Material for decal when we can deploy on a given location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Decal")
	UMaterialInterface* CanDeployOnMaterial;

	/* Material for decal when we cannot deploy on a given location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Decal")
	UMaterialInterface* CannotDeployOnMaterial;

	/* How large the decal should be. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Decal")
	FVector DecalSize;

	/* Mesh to use as preview arrow. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Preview Arrow")
	UStaticMesh* PreviewMesh;

	/* All preview mesh materials when placement is possible. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Preview Arrow")
	TArray<UMaterialInterface*> CanPlacePreviewMeshMaterials;

	/* All preview mesh materials when placement is not possible. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Preview Arrow")
	TArray<UMaterialInterface*> CannotPlacePreviewMeshMaterials;

	/* Additional rotation to add to mesh rotation (mesh is rotated to face spline). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Preview Arrow")
	FRotator AdditionalMeshRotation;

	/* How far midpoint should be above ground, which determines ark size. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Preview Arrow")
	float ZOffset;

	/* How large should the preview mesh be. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Preview Arrow")
	float MeshScale;

	/* How far apart meshes should be. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Redeploy Preview|Preview Arrow")
	float MeshSpacing;

/////////////////////////////////////////////////////////////////////////////////
//// INTENAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Internal value of the owner. */
	EEntity OwnerInternal;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	ARedeployPreview();

	/**
	 * SERVER-ONLY.
	 * Function which should be called when we want to assign an owner and configure
	 * necessary delegates.
	 */
	void ConfigureRedployPreview(EEntity WhoOwns);

/////////////////////////////////////////////////////////////////////////////////
//// VISUALISATION
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Used to update visualisation when mouse-to-grid location changes.
	 * @param WhoChanged Who actually changed the mouse to grid location.
	 * @param bLocationValid Whether this was a valid loction on the grid.
	 * @param NewLocation World location on grid.
	 * @param NewGridIndex Actual grid index.
	 */
	UFUNCTION()
	void MouseToGridChanged(EEntity WhoChanged, bool bLocationValid, const FVector& NewLocation, const FIntPair& NewGridIndex);

	/**
	 * Updates visibility on server and client appropriately (i.e. hides on non-owner).
	 * @param ActualOwner Who is the actual owner of this preview (as configured).
	 * @param bLocationValid Whether this was a valid loction on the grid.
	 * @param NewLocation World location on grid.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateVisualisation(
		EEntity ActualOwner, 
		bool bLocationValid, 
		bool CanDeploy, 
		const FVector& NewLocation,
		const FVector& StartLocation,
		float NewZPointOffset,
		float NewMeshTransformScale,
		float NewMeshSpacing,
		UStaticMesh* NewPreviewMesh,
		const TArray<UMaterialInterface*>& NewPreviewMeshMaterials,
		const FRotator& NewAdditionalRotation
	);

/////////////////////////////////////////////////////////////////////////////////
//// UTILITY
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Gets midpoint between start and end, with Z value increased by MidpointZOffset.
	 * @param Start Start point.
	 * @param End Endpoint.
	 * @param MidpointZOffset Midpoint Z + this = final Z.
	 * @return Mid point between start and end, with Z value increased by MidpointZOffset.
	 */
	FVector GetMidpointWithOffset(const FVector& Start, const FVector& End, float MidpointZOffset);

	template <class T>
	void DestroyAllComponentsOfClass()
	{
		TArray<T*> Components;
		GetComponents<T>(Components);

		if (Components.Num() > 0)
		{
			for (int32 i = 0; i < Components.Num(); i++) Components[i]->DestroyComponent();
		}
	}
};
