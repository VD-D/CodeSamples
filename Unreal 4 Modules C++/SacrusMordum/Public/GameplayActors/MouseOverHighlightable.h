// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayActors/MouseSelectable.h"
#include "Interfaces/HighlightableInterface.h"
#include "MouseOverHighlightable.generated.h"

/* Class which has all its static mesh components render in custom depth on mouse over. */
UCLASS()
class SACRUSMORDUM_API AMouseOverHighlightable : public AMouseSelectable, public IHighlightableInterface
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- COMPONENTS --- */

	/* Initial static mesh. Can add more. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mouse Over Highlightable")
	class UStaticMeshComponent* StaticMesh;

private:
	/* Force disables custom depth rendering. */
	bool bForceDisableCustomDepthRender;
	bool bHasMouseEntered;
	
/* --- FUNCTIONS --- */
public:
	/** Constructor.
	  */
	AMouseOverHighlightable();

	/** Prevents custom depth highlighting on mouse over until EnableCustomDepthHighlighting() is called.
	  */
	UFUNCTION(BlueprintCallable, Category = "Mouse Over Highlightable")
	void ForceDisableCustomDepthHighlighting();

	/** Re-enables custom depth highlighting if ForceDisableCustomDepthHighlighting() was called.
	  */
	UFUNCTION(BlueprintCallable, Category = "Mouse Over Highlightable")
	void EnableCustomDepthHighlighting();

protected:
	/** Called when mouse over happens.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	virtual void OnMouseOver_Implementation(class UPrimitiveComponent* TouchedComponent) override;

	/** Called when mouse over ends.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	virtual void OnMouseOverEnd_Implementation(class UPrimitiveComponent* TouchedComponent) override;

	/** IHighlightableInterface implementation.
	  * @param Enable - Whether or not to enable or disable highlights.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Highlightable Interface")
	void SetEnableHighlights(bool Enable);
	virtual void SetEnableHighlights_Implementation(bool Enable) override;

	/** Utility function for setting all static mesh components to render in custom depth.
	  * Can be overriden if a different method for rendering in custom depth is needed.
	  * @param RenderInCustomDepth - True to render in custom depth.
	  */
	virtual void SetComponentRenderCustomDepth(bool RenderInCustomDepth);
};
