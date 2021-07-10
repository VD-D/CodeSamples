// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/LineOfSightVisualiser.h"
#include "Interfaces/LineOfSightParameterInterface.h"
#include "Materials/Material.h"
#include "ProceduralMeshComponent.h"

ULineOfSightVisualiser::ULineOfSightVisualiser()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SubdivisionCount = 3;
	IterationsCount = 3;
	MeshResolution = 0.15;
	MinEdgeIdentifierDistance = 300.0f;
	VisionInterpRate = 15.0f;
	VisualisationType = EVisualisation::Peripheral;
	TraceChannel = TEnumAsByte<ECollisionChannel>(ECollisionChannel::ECC_Visibility);
}

void ULineOfSightVisualiser::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateVisualisation(DeltaTime);
}

void ULineOfSightVisualiser::DisableVisualisation()
{
	SetComponentTickEnabled(false);
	VisualisationMesh->SetHiddenInGame(true);
	bWasObstacleDetectedInCurrentCycle = false;
}

void ULineOfSightVisualiser::ScaleVisibility_Implementation(float & Scale)
{
	TargetVisionRadius = MaxVisionRadius * Scale;
}

void ULineOfSightVisualiser::BeginPlay()
{
	Super::BeginPlay();

	Params.bTraceComplex = false;
	Params.TraceTag = FName("LoS Trace");
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(GetOwner());

	InitaliseMesh();
}

void ULineOfSightVisualiser::InitaliseMesh()
{
	ILineOfSightParameterInterface* LoSParamInterface = Cast<ILineOfSightParameterInterface>(GetOwner());
	if (LoSParamInterface == nullptr || MeshResolution <= 0.0f || VisulisationMaterial == nullptr)
	{
		return;
	}

	bool bDoesHaveProceduralMeshComponent = false;
	TArray<UProceduralMeshComponent*> ProceduralMeshComponents;
	GetOwner()->GetComponents<UProceduralMeshComponent>(ProceduralMeshComponents);

	for (auto ProceduralMeshComponent : ProceduralMeshComponents)
	{
		if (ProceduralMeshComponent)
		{
			if (VisualisationMeshName == ProceduralMeshComponent->GetName())
			{
				VisualisationMesh = ProceduralMeshComponent;
				bDoesHaveProceduralMeshComponent = true;
				break;
			}
		}
	}

	if (!bDoesHaveProceduralMeshComponent)
	{
		return;
	}

	VisualisationMesh->SetMaterial(0, VisulisationMaterial);
	VisualisationMesh->SetMaterial(1, VisulisationMaterial);
	VisualisationMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	VisualisationMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	VisualisationMesh->SetCastShadow(false);

	FLineOfSightParameters CurrentLoSParams;
	LoSParamInterface->Execute_GetLineOfSightParameters(GetOwner(), CurrentLoSParams);

	float CurrentVisionAngle = VisualisationType == EVisualisation::Peripheral ? CurrentLoSParams.VisionAnglePeripheral : CurrentLoSParams.VisionAngleFull;
	NormalizedVisionAngle = FModChecked(CurrentVisionAngle, 360.0f) <= 0.0f ? (FModChecked(CurrentVisionAngle, 360.0f)) + 360.0f : FModChecked(CurrentVisionAngle, 360.0f);

	CurrentVisionRadius = VisualisationType == EVisualisation::Peripheral ? CurrentLoSParams.VisionRadiusPeripheral : CurrentLoSParams.VisionRadiusFull;
	TargetVisionRadius = CurrentVisionRadius;
	MaxVisionRadius = CurrentVisionRadius;

	MinimalLoSPoints = FMath::CeilToInt(NormalizedVisionAngle * MeshResolution) + 1;
	CorrectedMeshResolution = 1.0f / (NormalizedVisionAngle / (float)(MinimalLoSPoints - 1));
	MeshDivisionAngle = 1.0f / CorrectedMeshResolution;
	EdgeSubdivision = MeshDivisionAngle / (float)(SubdivisionCount);

	TArray<FVector> DefaultMeshVerticies;
	DefaultMeshVerticies.Init(FVector::ZeroVector, MinimalLoSPoints + 1);
	CreateMeshSection(0, DefaultMeshVerticies);
	bHasBeenInitialised = true;

	EnableVisualisation();
}

void ULineOfSightVisualiser::EnableVisualisation()
{
	if (!bHasBeenInitialised)
	{
		return;
	}

	if (!IsComponentTickEnabled())
	{
		bDoesMeshNeedToBeUpdated = true;
		UpdateVisualisation(0.0f);
		SetComponentTickEnabled(true);
		SetTickGroup(ETickingGroup::TG_LastDemotable);
	}
	VisualisationMesh->SetHiddenInGame(false);
}

void ULineOfSightVisualiser::UpdateVisualisation(float DeltaTime)
{
	if (!bHasBeenInitialised)
	{
		return;
	}

	if (CurrentVisionRadius != TargetVisionRadius)
	{
		CurrentVisionRadius = FMath::FInterpConstantTo(CurrentVisionRadius, TargetVisionRadius, DeltaTime, VisionInterpRate);
	}

	LineTraceCount = 0;
	bool bWasObstactleDetectedInPreviousCycle = bWasObstacleDetectedInCurrentCycle;
	TArray<FVector> CurrentPoints;

	GeneratePoints(bWasObstacleDetectedInCurrentCycle, CurrentPoints);

	UpdateMeshSection(0, CreateVertexDataFromPoints(CurrentPoints, true));
	if (CurrentPoints.Num() > MinimalLoSPoints)
	{
		CreateMeshSection(1, CreateVertexDataFromPoints(CurrentPoints, false));
	}
	else
	{
		VisualisationMesh->ClearMeshSection(1);
	}
}

void ULineOfSightVisualiser::CreateMeshSection(const int32 SectionIndex, const TArray<FVector> Vertices)
{
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> VertColors;

	VisualisationMesh->CreateMeshSection_LinearColor(SectionIndex, Vertices, CreateTriangleDataFromVertices(Vertices), Normals, UVs, VertColors, Tangents, false);
	VisualisationMesh->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
}

void ULineOfSightVisualiser::UpdateMeshSection(const int32 SectionIndex, const TArray<FVector> Vertices)
{
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> VertColors;

	VisualisationMesh->UpdateMeshSection_LinearColor(SectionIndex, Vertices, Normals, UVs, VertColors, Tangents);
	VisualisationMesh->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
}

TArray<int32> ULineOfSightVisualiser::CreateTriangleDataFromVertices(const TArray<FVector> Vertices)
{
	TArray<int32> ReturnVerticies;
	ReturnVerticies.Init(0, (Vertices.Num() * 3) + 3);
	for (int32 i = 0; i <= Vertices.Num() - 3; ++i)
	{
		ReturnVerticies[(i * 3)] = 0;
		ReturnVerticies[((i * 3) + 1)] = i + 1;
		ReturnVerticies[((i * 3) + 2)] = i + 2;
	}

	return ReturnVerticies;
}

TArray<FVector> ULineOfSightVisualiser::CreateVertexDataFromPoints(const TArray<FVector> Points, const bool ShouldFocusOnDefaultVerticies)
{
	int32 FirstIndex = 0;
	int32 LastIndex = 0;

	TArray<FVector> LocalVertices;

	if (ShouldFocusOnDefaultVerticies)
	{
		LastIndex = MinimalLoSPoints - 1;
	}
	else
	{
		FirstIndex = MinimalLoSPoints - 1;
		LastIndex = Points.Num() - 1;
	}

	FVector MeshLocation = VisualisationMesh->GetComponentLocation();
	// UProperty* Prop = this->GetClass()->FindPropertyByName(FName("LocalVertices"));
	// const UArrayProperty* ArrayProp = Cast<const UArrayProperty>(Prop);

	LocalVertices.Init(FVector::ZeroVector, LastIndex - FirstIndex + 2);

	for (int32 i = FirstIndex; i <= LastIndex; ++i)
	{
		FVector Current = Points[i];
		LocalVertices[i - FirstIndex + 1] = FVector(Current.X - MeshLocation.X, Current.Y - MeshLocation.Y, Current.Z - MeshLocation.Z);
	}

	return LocalVertices;
}

void ULineOfSightVisualiser::GeneratePoints(bool & bWasObstactleDetected, TArray<FVector>& Points)
{
	TArray<FVector> LocalPoints;
	FHitResult PreviousHitResult;
	float CurrentTraceAngle = NormalizedVisionAngle / 2.0f;
	float PreviousTraceAngle = 0.0f;
	bool bHasObstacleBeenDetected = false;

	for (int32 i = 1; i <= MinimalLoSPoints; ++i)
	{
		FHitResult Hit = SingleTrace(CurrentTraceAngle);
		if (i > 1)
		{
			bool bEdgeTest = PerformEdgeTest(PreviousHitResult, Hit);
			if (bEdgeTest)
			{
				bHasObstacleBeenDetected = true;
				TArray<FVector> EdgeFixes = ConstructEdgeFixApproximation(PreviousTraceAngle, PreviousHitResult);
				LocalPoints.Append(EdgeFixes);
			}
			else if (!bHasObstacleBeenDetected)
			{
				bHasObstacleBeenDetected = Hit.bBlockingHit;
			}
		}
		else
		{
			if (!bHasObstacleBeenDetected)
			{
				bHasObstacleBeenDetected = Hit.bBlockingHit;
			}
		}

		FVector BlockingPoint = BlockingTraceEnd(Hit);
		LocalPoints.Add(BlockingPoint);

		PreviousHitResult = Hit;
		PreviousTraceAngle = CurrentTraceAngle;
		CurrentTraceAngle = PreviousTraceAngle + (-1.0f * MeshDivisionAngle);
	}

	bWasObstactleDetected = bHasObstacleBeenDetected;
	Points = LocalPoints;
}

FHitResult ULineOfSightVisualiser::SingleTrace(const float TraceAngle)
{
	FVector RotatedVector = VisualisationMesh->GetComponentLocation() + (GetOwner()->GetActorForwardVector().RotateAngleAxis(TraceAngle, GetOwner()->GetActorUpVector()) * CurrentVisionRadius);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	FHitResult Hit;
	Hit.Init();

	bool bHit = GetWorld() != nullptr ? GetWorld()->LineTraceSingleByChannel(Hit, VisualisationMesh->GetComponentLocation(), RotatedVector,
	TraceChannel.GetValue(), Params) : false;

	LineTraceCount++;
	return Hit;
}

FVector ULineOfSightVisualiser::BlockingTraceEnd(FHitResult Hit)
{
	return Hit.bBlockingHit ? FVector(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z) : FVector(Hit.TraceEnd.X, Hit.TraceEnd.Y, Hit.TraceEnd.Z);
}

bool ULineOfSightVisualiser::IsMeshUpdateRequired(const bool ObstacleDetectedInPrevCycle, const bool ObstacleDetectedInCurrentCycle)
{
	if (bDoesMeshNeedToBeUpdated)
	{
		bDoesMeshNeedToBeUpdated = false;
		return true;
	}

	return (ObstacleDetectedInPrevCycle || ObstacleDetectedInCurrentCycle);
}

bool ULineOfSightVisualiser::PerformEdgeTest(const FHitResult StartingHit, const FHitResult EndingHit)
{
	return ((StartingHit.GetActor() != EndingHit.GetActor()) || (FMath::Abs(StartingHit.Distance - EndingHit.Distance) > MinEdgeIdentifierDistance));
}

TArray<FVector> ULineOfSightVisualiser::ConstructEdgeFixApproximation(const float StartingAngle, const FHitResult StartingHit)
{
	TArray<FVector> NewPoints;
	FHitResult CurrentHitResult;
	FHitResult PreviousHitResult = StartingHit;
	FVector StartingPoint = FVector::ZeroVector;
	float CurrentTraceAngle = 0.0f;
	float PreviousTraceAngle = StartingAngle;

	for (int32 i = 1; i <= SubdivisionCount; ++i)
	{
		CurrentTraceAngle = PreviousTraceAngle + (-1.0f * EdgeSubdivision);
		StartingPoint = BlockingTraceEnd(PreviousHitResult);

		if (i > 1)
		{
			NewPoints.Add(StartingPoint);
		}

		CurrentHitResult = SingleTrace(CurrentTraceAngle);
		bool bEdgeTest = PerformEdgeTest(PreviousHitResult, CurrentHitResult);

		if (bEdgeTest)
		{
			FVector UpdatedStartPointLocal = FVector::ZeroVector;
			FVector UpdatedEndPointLocal = FVector::ZeroVector;
			bool bWasStartUpdated = false;
			bool bWasEndUpdated = false;
			RecalculateEdgePoints(PreviousHitResult, PreviousTraceAngle, CurrentTraceAngle, BlockingTraceEnd(PreviousHitResult), BlockingTraceEnd(CurrentHitResult), bWasStartUpdated, UpdatedStartPointLocal, bWasEndUpdated, UpdatedEndPointLocal);

			if (bWasStartUpdated)
			{
				NewPoints.Add(UpdatedStartPointLocal);
			}

			if (bWasEndUpdated)
			{
				NewPoints.Add(UpdatedEndPointLocal);
			}
		}

		PreviousTraceAngle = CurrentTraceAngle;
		PreviousHitResult = CurrentHitResult;
	}

	return NewPoints;
}

void ULineOfSightVisualiser::RecalculateEdgePoints(const FHitResult StartingHit, const float StartingAngle, const float EndingAngle, 
	const FVector StartingPoint, const FVector EndingPoint, bool & StartUpdated, FVector & UpdatedStartPoint, bool & EndUpdated, 
	FVector & UpdatedEndPoint)
{
	FVector LocalStartPoint = StartingPoint;
	FVector LocalEndPoint = EndingPoint;

	float CurrentTraceAngle = 0.0f;
	float LocalStartingAngle = StartingAngle;
	float LocalEndingAngle = EndingAngle;

	for (int32 i = 1; i <= SubdivisionCount; ++i)
	{
		CurrentTraceAngle = (LocalStartingAngle + LocalEndingAngle) / 2.0f;
		FHitResult EndingHit = SingleTrace(CurrentTraceAngle);
		bool bEdgeTest = PerformEdgeTest(StartingHit, EndingHit);

		if (bEdgeTest)
		{
			LocalEndPoint = BlockingTraceEnd(EndingHit);
			LocalEndingAngle = CurrentTraceAngle;
			EndUpdated = true;
		}
		else
		{
			LocalStartPoint = BlockingTraceEnd(EndingHit);
			LocalStartingAngle = CurrentTraceAngle;
			StartUpdated = true;
		}
	}

	UpdatedStartPoint = LocalStartPoint;
	UpdatedEndPoint = LocalEndPoint;
}

float ULineOfSightVisualiser::FModChecked(float A, float B)
{
	return (B != 0.0f) ? FMath::Fmod(A, B) : 0.0f;
}
