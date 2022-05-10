// Fill out your copyright notice in the Description page of Project Settings.

/* Class header */
#include "Actors/UniformISMSpawner.h"

/* Other includes. */
#include "Components/SceneComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

AUniformISMSpawner::AUniformISMSpawner()
{
	/* Component creation. */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	RootComponent = SceneRoot;

	/* Reasonable defaults. */
	bRebuild = false;
	PrimaryActorTick.bCanEverTick = false;
	Extent = FIntPoint(6, 2);
	XYDimensions = FVector2D(200.0f, 200.0f);

	InstancedMeshCollisionEnabled = TEnumAsByte<ECollisionEnabled::Type>(ECollisionEnabled::Type::QueryAndPhysics);
	CollisionObjectType = TEnumAsByte<ECollisionChannel>(ECollisionChannel::ECC_WorldStatic);
}

void AUniformISMSpawner::BuildInstancedMesh()
{
	/* Clear all instances from all components and destroy the components. */
	for (auto ISMComp : ISMComps) 
	{
		if (IsValid(ISMComp.Value)) 
		{
			ISMComp.Value->ClearInstances();
			ISMComp.Value->DestroyComponent();
		}
	}

	ISMComps.Empty(Meshes.Num());

	/* Do nothing if we have no meshes to spawn. */
	if (Meshes.Num() == 0) 
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : No meshes assigned! Please assign at least 1."), *GetDebugName(this)));
		return;
	}

	/* Create new components. */
	for (int32 i = 0; i < Meshes.Num(); i++)
	{
		if (!ISMComps.Contains(i))
		{
			UInstancedStaticMeshComponent* NewISM = NewObject<UInstancedStaticMeshComponent>(this, UInstancedStaticMeshComponent::StaticClass(), NAME_None);
			NewISM->SetupAttachment(RootComponent);
			NewISM->RegisterComponent();
			NewISM->SetCollisionEnabled(InstancedMeshCollisionEnabled.GetValue());
			NewISM->SetCollisionObjectType(CollisionObjectType.GetValue());
			NewISM->SetCollisionResponseToChannels(CollisionResponses);
			ISMComps.Add(i, NewISM);
		}
	}

	FTransform MeshTransform;
	FVector MeshLocation;
	for (int32 i = 0; i < FMath::Abs(Extent.X); i++)
	{
		for (int32 j = 0; j < FMath::Abs(Extent.Y); j++)
		{
			/* If our x/y 'coordinate' is meant to be ignored, then continue. */
			if (IgnoreIndicies.Contains(FIntPoint(i, j))) continue;

			/* Pick one of the possible meshes at random. */
			int32 RandomIndex = FMath::RandRange(0, Meshes.Num() - 1);
			FRandomisedMeshProperties NewProperties = Meshes[RandomIndex];

			/* This should not happen, but the check is added just in case to not dereference a nullptr. */
			if (!ISMComps.Contains(RandomIndex)) continue;

			/* Select the ISM which we want to spawn components on. */
			UInstancedStaticMeshComponent* InstancedStaticMeshComponent = *ISMComps.Find(RandomIndex);
			InstancedStaticMeshComponent->SetStaticMesh(NewProperties.Mesh);

			/* Computes the location of the current instance, depending if we are building horizontally or vertically. */
			MeshLocation.X = i * XYDimensions.X;
			MeshLocation.Y = bConstructVertically ? 0.0f : j * XYDimensions.Y;
			MeshLocation.Z = bConstructVertically ? j * XYDimensions.Y : 0.0f;

			/* Sets up the transform for the current instance. */
			MeshTransform.SetLocation(MeshLocation);
			MeshTransform.SetRotation(FQuat(NewProperties.RandomisedRotator()));
			MeshTransform.SetScale3D(NewProperties.RandomisedScale());

			/* Add the instance. */
			InstancedStaticMeshComponent->AddInstance(MeshTransform);
		}
	}
}

void AUniformISMSpawner::OnConstruction(const FTransform& Transform)
{
	/* Build the mesh via the construction script if specified to do so*/
	if (bRebuild) 
	{
		BuildInstancedMesh();
		bRebuild = false;
	}
}


