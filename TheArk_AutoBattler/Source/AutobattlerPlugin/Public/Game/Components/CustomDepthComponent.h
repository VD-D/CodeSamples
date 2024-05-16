// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomDepthComponent.generated.h"

/**
 * Component responsible for enabling/disabling rendering in custom depth
 * and locking that state.
 */
UCLASS(ClassGroup=(Autobattler), meta=(BlueprintSpawnableComponent))
class AUTOBATTLERPLUGIN_API UCustomDepthComponent : public UActorComponent
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Current custom depth stencil value. */
	int32 CustomDepthStencil;

	/* Whether showing or hiding locked. */
	bool IsLocked;

public:	
	/**
	 * Default constructor.
	 */
	UCustomDepthComponent();

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Setter for custom depth stencil.
	 * @param NewCustomDepthStencil New custom depth stencil value to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom Depth Component")
	void SetCustomDepthStencil(int32 NewCustomDepthStencil) { CustomDepthStencil = NewCustomDepthStencil; }

	/**
	 * Sets enabling locking. If locked, will never stop rendering in custom depth even if SetNewRenderCustomDepth
	 * is called to stop showing. 
	 * @param NewLock True to enable locking.
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom Depth Component")
	void SetLockRenderCustomDepth(bool NewLock) { IsLocked = NewLock; }

	/**
	 * Getter for whether or not locking is enabled.
	 * @return Is locking enabled.
	 */
	UFUNCTION(BlueprintPure, Category = "Custom Depth Component")
	bool GetIsLocked() const { return IsLocked; }

/////////////////////////////////////////////////////////////////////////////////
//// CUSTOM DEPTH STENCIL COMPONENT API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Enables/Disables rendering in custom depth.
	 * @param NewRender True to render in custom depth, false to stop rendering in custom depth.
	 * @param SkeletalMeshes Render skeletal meshes in custom depth.
	 * @param StaticMeshes Render static meshes in custom depth.
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom Depth Component")
	void SetNewRenderCustomDepth(bool NewRender, bool SkeletalMeshes = true, bool StaticMeshes = false);	
};
