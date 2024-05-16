// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Components/CustomDepthComponent.h"

/* Engine includes. */
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

UCustomDepthComponent::UCustomDepthComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCustomDepthComponent::SetNewRenderCustomDepth(bool NewRender, bool SkeletalMeshes, bool StaticMeshes)
{
	if (!IsValid(GetOwner())) return;
	if (SkeletalMeshes)
	{
		TArray<USkeletalMeshComponent*> SkeletalMeshComps;
		GetOwner()->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);

		for (auto SkeletalMeshComp : SkeletalMeshComps) 
		{
			SkeletalMeshComp->SetCustomDepthStencilValue(CustomDepthStencil);
			SkeletalMeshComp->SetRenderCustomDepth(NewRender || IsLocked);
		}
	}

	if (StaticMeshes)
	{
		TArray<UStaticMeshComponent*> StaticMeshComps;
		GetOwner()->GetComponents<UStaticMeshComponent>(StaticMeshComps);

		for (auto StaticMeshComp : StaticMeshComps) 
		{
			StaticMeshComp->SetCustomDepthStencilValue(CustomDepthStencil);
			StaticMeshComp->SetRenderCustomDepth(NewRender || IsLocked);
		}
	}
}
