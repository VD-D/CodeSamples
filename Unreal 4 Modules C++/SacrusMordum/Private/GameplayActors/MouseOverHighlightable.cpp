// Copyright Robert Zygmunt Uszynski 2020-2021

#include "GameplayActors/MouseOverHighlightable.h"
#include "Components/StaticMeshComponent.h"

AMouseOverHighlightable::AMouseOverHighlightable()
{
	/* Component Creation */
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
}

void AMouseOverHighlightable::ForceDisableCustomDepthHighlighting()
{
	/* Do nothing if force disable is already enabled. */
	if (bForceDisableCustomDepthRender) return;

	/* Turn off custom depth render and set bForceDisableCustomDepthRender to true. */
	SetComponentRenderCustomDepth(false);
	bForceDisableCustomDepthRender = true;
}

void AMouseOverHighlightable::EnableCustomDepthHighlighting()
{
	/* Do nothing if force disable is already disabled. */
	if (!bForceDisableCustomDepthRender) return;

	/* If the mouse has entered the selection bounds, re-enable highlighting. Then set bForceDisableCustomDepthRender to false. */
	if (bHasMouseEntered) SetComponentRenderCustomDepth(true);
	bForceDisableCustomDepthRender = false;
}

void AMouseOverHighlightable::OnMouseOver_Implementation(UPrimitiveComponent * TouchedComponent)
{
	bHasMouseEntered = true;
	if (!bForceDisableCustomDepthRender) SetComponentRenderCustomDepth(true);
}

void AMouseOverHighlightable::OnMouseOverEnd_Implementation(UPrimitiveComponent * TouchedComponent)
{
	bHasMouseEntered = false;
	if (!bForceDisableCustomDepthRender) SetComponentRenderCustomDepth(false);
}

void AMouseOverHighlightable::SetEnableHighlights_Implementation(bool Enable)
{
	if (bForceDisableCustomDepthRender) EnableCustomDepthHighlighting();
	else ForceDisableCustomDepthHighlighting();
}

void AMouseOverHighlightable::SetComponentRenderCustomDepth(bool RenderInCustomDepth)
{
	/* Get all static mesh components. */
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	/* Set all static mesh components to render in custom depth. */
	for (auto StaticMeshComp : StaticMeshComponents)
	{
		if (StaticMeshComp == nullptr) continue;

		StaticMeshComp->SetRenderCustomDepth(RenderInCustomDepth);
	}
}