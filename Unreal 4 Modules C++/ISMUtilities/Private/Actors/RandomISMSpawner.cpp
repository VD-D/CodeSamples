// Fill out your copyright notice in the Description page of Project Settings.

/* Class header */
#include "Actors/RandomISMSpawner.h"

/* Other includes */
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"

ARandomISMSpawner::ARandomISMSpawner()
{
	/* Component Creation */
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Billboard"));
	RootComponent = SceneComponent;

	/* Reasonable Defaults */
	SpawnShape = EBoundsType::Box;
	BoxExtent = FVector(250.0f);
	CylinderRadius = 250.0f;
	CylinderExtent = 250.0f;
	SpawnOnCollisionChannel = TEnumAsByte<ECollisionChannel>(ECollisionChannel::ECC_Visibility);
	bUseRandomSeed = true;
	Seed = 12345;
	Spread = 1.0f;
	MaxSpawnAngle = 40.0f;
	SpawnCount = 50;
	MaxAdjustmentIterations = 50;

	bCastShadows = true;
	CollisionType = TEnumAsByte<ECollisionEnabled::Type>(ECollisionEnabled::Type::QueryAndPhysics);

 	bDrawDebugShapes = true;
	DebugShapesLineThickness = 5.0f;
	DebugShapesColor = FLinearColor::Yellow;
	CutShapesLineThickness = 2.0f;
	CutShapesColor = FLinearColor::Red;
	
	PrimaryActorTick.bCanEverTick = true;
}

void ARandomISMSpawner::SpawnRandomisedMeshes()
{
	/* Ensure world is valid. */
	if (!IsValid(GetWorld())) 
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : Trying to spawn in invalid world!"), *GetDebugName(this)));
		return;
	}

	/* Initialise seed and removed previous debug lines if any where created. */
	StreamSeed.Initialize(bUseRandomSeed ? Seed : FMath::Rand());

	/* Do nothing further if the user has not added mesh to spawn. */
	if (MeshTemplates.Num() < 1)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : Need at least one mesh to spawn!"), *GetDebugName(this)));
		return;
	}

	/* Clear all current HISM (Hierarchical Instanced Static Meshes) */
	for (int32 i = 0; i < HISMs.Num(); i++)
	{
		if (IsValid(HISMs[i])) HISMs[i]->DestroyComponent();
		HISMs[i] = nullptr;
	}

	HISMs.Empty(MeshTemplates.Num());

	/* Configure new HISMs and assign static meshes. Number of HISMs == Length of MeshTemplates. */
	for (auto MeshTemplate : MeshTemplates)
	{
		UHierarchicalInstancedStaticMeshComponent* NewHISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, UHierarchicalInstancedStaticMeshComponent::StaticClass(), NAME_None);
		NewHISM->SetupAttachment(RootComponent);
		NewHISM->RegisterComponent();
		NewHISM->SetStaticMesh(MeshTemplate.Mesh);
		NewHISM->SetCastShadow(bCastShadows);
		NewHISM->SetCullDistance(MaxDrawDistance);
		NewHISM->SetCullDistances(0, HISMMaxCullDistance);
		NewHISM->SetCollisionEnabled(CollisionType);
		NewHISM->SetCollisionResponseToChannels(CollisionResponses);
		HISMs.Emplace(NewHISM);
	}

	int32 CountInteral = 0;

	for (int32 i = 0; i < FMath::Abs(SpawnCount); i++)
	{
		/* Pseudo-random index is taken from the StreamSeed, while true random uses FMath's RandRange. */
		int32 RandomIndex = bUseRandomSeed ? StreamSeed.RandRange(0, HISMs.Num() - 1) : FMath::RandRange(0, HISMs.Num() - 1);

		/* This should never be the case, but if some mistake is made, a warning message is printed. */
		if (!MeshTemplates.IsValidIndex(RandomIndex) || !HISMs.IsValidIndex(RandomIndex))
		{
			if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::Yellow, FString::Printf(TEXT("%s : Index %d is outside of valid MeshProperties indicies!"), *GetDebugName(this), RandomIndex));
			continue;
		}

		FRandomMeshTemplate SelectedTemplate = MeshTemplates[RandomIndex];
		UHierarchicalInstancedStaticMeshComponent* SelectedHISM = HISMs[RandomIndex];
		
		/* Use our random rotator/scale structs to generate a desired rotator and scale. */
		FVector DesiredScale = SelectedTemplate.RandomScaleProperties.GenerateRandomScale(StreamSeed);
		FRotator DesiredRotation = SelectedTemplate.RandomRotationProperties.GenerateRandomRotator(StreamSeed);

		FVector RandomLoc = FVector::ZeroVector;
		FVector ImpactNormal = FVector::ZeroVector;

		NumberIterationsExpired = 0;
		bool bSuccess = RandomPointChecked(RandomLoc, ImpactNormal, DesiredRotation, DesiredScale, SelectedHISM);

		/* This condition indicates we have run out of iterations to try and place meshes. */
		if (!bSuccess)
		{
			if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%s : Managed to spawn %d before failing."), *GetDebugName(this), CountInteral));
			break;
		}

		CountInteral += 1;

		FTransform WorldSpaceTransform;
		WorldSpaceTransform.SetLocation(RandomLoc);
		WorldSpaceTransform.SetScale3D(DesiredScale);
		if (bAlignToNormal)
		{
			WorldSpaceTransform.SetRotation(FQuat(FRotationMatrix::MakeFromZX(ImpactNormal, GetActorUpVector()).Rotator()));
			WorldSpaceTransform.ConcatenateRotation(DesiredRotation.Quaternion());
		}
		else WorldSpaceTransform.SetRotation(DesiredRotation.Quaternion());

		int32 InstanceIndex = SelectedHISM->AddInstanceWorldSpace(WorldSpaceTransform);
		SelectedHISM->UpdateInstanceTransform(InstanceIndex, WorldSpaceTransform, true);
	}
}

void ARandomISMSpawner::OnConstruction(const FTransform& Transform)
{
	FlushPersistentDebugLines(GetWorld());

	/* Draw box or cylinder debug shape if so desired. */
	if (bDrawDebugShapes)
	{
		switch (SpawnShape)
		{
			case EBoundsType::Box:
				DrawDebugBox(GetWorld(), GetActorLocation(), BoxExtent, DebugShapesColor.ToFColor(false), true, -1.0f, 0, DebugShapesLineThickness);
				break;

			case EBoundsType::Cylinder:
			{
				FVector StartLocation;
				FVector EndLocation;
				float CylRadius;
				MakeCylinderShape(CylRadius, StartLocation, EndLocation);
				DrawDebugCylinder(GetWorld(), StartLocation, EndLocation, CylRadius, 100, DebugShapesColor.ToFColor(false), true, -1.0f, 0, DebugShapesLineThickness);
			}
			break;
		}
	}

	if (bRegenerate)
	{
		SpawnRandomisedMeshes();
		bRegenerate = false;
	}
}

void ARandomISMSpawner::MakeCylinderShape(float& NewCylinderRadius, FVector& StartLocation, FVector& EndLocation)
{
	FVector CurrentWorldLocation = GetActorLocation();
	NewCylinderRadius = FMath::Abs(CylinderRadius);
	StartLocation = FVector(CurrentWorldLocation.X, CurrentWorldLocation.Y, CurrentWorldLocation.Z + FMath::Abs(CylinderExtent));
	EndLocation = FVector(CurrentWorldLocation.X, CurrentWorldLocation.Y, CurrentWorldLocation.Z - FMath::Abs(CylinderExtent));
}

FVector ARandomISMSpawner::InteralGetRandomPointInBox()
{
	if (bUseRandomSeed)
	{
		/* To get a pseudo-random point, first we need the Max BoxExtent (which is just the BoxExtent) and its inverse, which is BoxExtent * -1.0f.
		We pick a random point between the Xs, Ys and Zs of these two points from stream, then offset it by the actor location. */
		FVector InverseExtent = BoxExtent * -1.0f;
		FVector RandPointStream = FVector(RandomPointInRangeFromStreamSeed(InverseExtent.X, BoxExtent.X), 
		RandomPointInRangeFromStreamSeed(InverseExtent.Y, BoxExtent.Y), RandomPointInRangeFromStreamSeed(InverseExtent.Z, BoxExtent.Z));
		return RandPointStream + GetActorLocation();
	}
	/* True randomness just gets a completely random point in a FBox. */
	else return FMath::RandPointInBox(FBox(GetActorLocation() - BoxExtent, GetActorLocation() + BoxExtent));
}

FVector ARandomISMSpawner::InternalGetRandomPointInSphere()
{
	float InverseRadius = FMath::Abs(CylinderRadius) * -1.0f;
	/* Pseudo-random gets random number from stream. */
	if (bUseRandomSeed) return FVector(RandomPointInRangeFromStreamSeed(InverseRadius, CylinderRadius), RandomPointInRangeFromStreamSeed(InverseRadius, CylinderRadius), 0.0f) + GetActorLocation();
	/* True randomness just uses RandRange. */
	else return FVector(FMath::RandRange(InverseRadius, CylinderRadius), FMath::RandRange(InverseRadius, CylinderRadius), 0.0f) + GetActorLocation();
}

bool ARandomISMSpawner::RandomPointChecked(FVector& FinalRandomLocation, FVector& ImpactNormal, const FRotator& Rotation, const FVector& Scale, UHierarchicalInstancedStaticMeshComponent* HISMToUse)
{
	/* Break condition. Effectively expires after MaxAdjustmentIterations attempts. */
	NumberIterationsExpired += 1;
	if (NumberIterationsExpired > MaxAdjustmentIterations) return false;

	/* Firstly, generate a completely random point. */
	FVector RandomPoint = SpawnShape == EBoundsType::Box ? InteralGetRandomPointInBox() : InternalGetRandomPointInSphere();

	/* Next, we need to get the actual mesh bounds. */
	if (HISMToUse->GetStaticMesh() == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(2, 15.0f, FColor::Red, FString::Printf(TEXT("%s : One or more meshes in MeshTemplates is null."), *GetDebugName(this)));
		return false;
	}

	FBoxSphereBounds MeshBounds = HISMToUse->GetStaticMesh()->GetBounds();
	FVector Bounds = MeshBounds.BoxExtent * Spread;

	/* Next, we want to start tracing from the top of the shape to the bottom. */
	FVector StartTrace = SpawnShape == EBoundsType::Box ? FVector(RandomPoint.X, RandomPoint.Y, GetActorLocation().Z + FMath::Abs(BoxExtent.Z)) : FVector(RandomPoint.X, RandomPoint.Y, GetActorLocation().Z + FMath::Abs(CylinderExtent));
	FVector EndTrace = SpawnShape == EBoundsType::Box ? FVector(RandomPoint.X, RandomPoint.Y, GetActorLocation().Z - FMath::Abs(BoxExtent.Z)) : FVector(RandomPoint.X, RandomPoint.Y, GetActorLocation().Z - FMath::Abs(CylinderExtent));

	/* Setup collision query params. */
	FCollisionQueryParams CollisionQueryParams;
	FCollisionQueryParams CollisionQueryParamsCopy;
	CollisionQueryParams.bReturnFaceIndex = false;
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParamsCopy = CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActors(SpawnOnActors);

	/* The first trace sees if we are hitting anything that we do not wish to spawn on. */
	FHitResult HitResult;
	bool bHitOne = GetWorld()->SweepSingleByChannel(HitResult, StartTrace, EndTrace, Rotation.Quaternion(), SpawnOnCollisionChannel.GetValue(), 
	FCollisionShape::MakeBox(Bounds * Scale), CollisionQueryParams);

	/* If this has occurred, we have hit an item which we do not wish to spawn on, so try again. */
	if (bHitOne) return RandomPointChecked(FinalRandomLocation, ImpactNormal, Rotation, Scale, HISMToUse);

	/* If this trace occurs, it implies that we are only going to hit actors we wish to spawn on. */
	bool bHitTwo = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, SpawnOnCollisionChannel.GetValue(), CollisionQueryParamsCopy);
	if (!bHitTwo) return RandomPointChecked(FinalRandomLocation, ImpactNormal, Rotation, Scale, HISMToUse);

	/* This condition implies that the max spawn angle has been exceeded. */
	if (FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector(0.0f, 0.0f, 1.0f))) >= MaxSpawnAngle) return RandomPointChecked(FinalRandomLocation, ImpactNormal, Rotation, Scale, HISMToUse);

	/* If this condition is reached, we have found a valid location to spawn on. */
	FinalRandomLocation = FVector(RandomPoint.X, RandomPoint.Y, HitResult.ImpactPoint.Z);
	ImpactNormal = HitResult.ImpactNormal;
	return true;
}

float ARandomISMSpawner::RandomPointInRangeFromStreamSeed(float Min, float Max)
{
	return Min + (Max - Min) * StreamSeed.FRand();
}
