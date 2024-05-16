// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Grid/AutobattlerGrid.h"

/* Autobattler includes. */
#include "Core/AutobattlerManager.h"

/* Engine includes. */
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

AAutobattlerGrid::AAutobattlerGrid()
{
	GridRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GridRoot"));
	RootComponent = GridRoot;

	GridXSize = 5;
	GridYSize = 5;
	MaxZExtent = 1000.0f;
	XYSize = 400.0f;
	DecalShrinkFactor = 0.0f;
	HiddenByDefault = true;
}

void AAutobattlerGrid::BuildGridInEditor()
{
	BuildGrid();
}

void AAutobattlerGrid::BeginPlay()
{
	Super::BeginPlay();
	BuildGrid();
	BindManagerDelegates();
	if (HiddenByDefault) ToggleGridVisibility(false);
	IsCurrentlyHidden = HiddenByDefault;
}

void AAutobattlerGrid::BuildGrid()
{
	GridIndexInfo.Empty(FMath::Max(GridXSize, 1) * FMath::Max(GridYSize, 1));

	TArray<UDecalComponent*> DecalComponents;
	GetComponents<UDecalComponent>(DecalComponents);

	for (int32 i = 0; i < DecalComponents.Num(); i++)
	{
		DecalComponents[i]->DestroyComponent();
	}

	float XYSizeClamped = FMath::Max(XYSize, 1.0f);

	FVector InitialLocation = GetActorLocation();
	FVector SpawnLocation = FVector::ZeroVector;
	FHitResult HitResult;
	FAttachmentTransformRules DecalAttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);

	for (int32 x = 0; x < FMath::Max(GridXSize, 1); x++)
	{
		for (int32 y = 0; y < FMath::Max(GridYSize, 1); y++)
		{
			if (GridIndexToLocation(FIntPair(x, y), SpawnLocation, HitResult))
			{
				UDecalComponent* NewDecalComponent = NewObject<UDecalComponent>(this, UDecalComponent::StaticClass());
				NewDecalComponent->RegisterComponent();
				NewDecalComponent->AttachToComponent(RootComponent, DecalAttachmentTransformRules);
				NewDecalComponent->SetWorldLocation(SpawnLocation);
				NewDecalComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
				NewDecalComponent->SetMaterial(0, DefaultDecalMaterial);
				NewDecalComponent->DecalSize = FVector(MaxZExtent, (XYSizeClamped / 2.0f) + DecalShrinkFactor, (XYSizeClamped / 2.0f) + DecalShrinkFactor);

				GridIndexInfo.Emplace(FIndexInfo(NewDecalComponent, FIntPair(x, y)));
			}
		}
	}
}

void AAutobattlerGrid::BindManagerDelegates()
{
	if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
	{
		// Manager->OnFloatBegun.AddDynamic(this, &);
		// 
	}
}

bool AAutobattlerGrid::IsValidGridIndex(const FIntPair& TestGridIndex) const
{
	return TestGridIndex.X >= 0 && TestGridIndex.Y >= 0 && TestGridIndex.X < GridXSize && TestGridIndex.Y < GridYSize;
}

bool AAutobattlerGrid::LocationToGridIndex(const FVector& Location, FIntPair& OutGridIndex) const
{
	if (Location.Z < GetActorLocation().Z + FMath::Abs(MaxZExtent) * -1.0f || Location.Z > GetActorLocation().Z + FMath::Abs(MaxZExtent))
	{
		OutGridIndex = FIntPair(-1, -1);
		return false;
	}

	FIntPair TestLocation = FIntPair(
		FMath::Floor((Location.X - GetActorLocation().X) / GetGridHeight() * GridXSize),
		FMath::Floor((Location.Y - GetActorLocation().Y) / GetGridWidth() * GridYSize)
	);

	if (IsValidGridIndex(TestLocation))
	{
		OutGridIndex = TestLocation;
		return true;
	}
	else
	{
		OutGridIndex = FIntPair(-1, -1);
		return false;
	}
}

bool AAutobattlerGrid::GridIndexToLocation(const FIntPair& TestGridIndex, FVector& Location, FHitResult& HitResult, TEnumAsByte<ECollisionChannel> TraceChannel, bool TraceComplex) const
{
	if (!IsValidGridIndex(TestGridIndex))
	{
		Location = FVector::ZeroVector;
		HitResult = FHitResult();
		return false;
	}

	Location.X = (TestGridIndex.X * XYSize + GetActorLocation().X) + (XYSize / 2.0f);
	Location.Y = (TestGridIndex.Y * XYSize + GetActorLocation().Y) + (XYSize / 2.0f);
	Location.Z = GetActorLocation().Z;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bTraceComplex = TraceComplex;

	if (IsValid(GetWorld()))
	{
		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			FVector(Location.X, Location.Y, Location.Z + FMath::Abs(MaxZExtent)),
			FVector(Location.X, Location.Y, Location.Z + FMath::Abs(MaxZExtent) * -1.0f),
			TraceChannel.GetValue(),
			CollisionQueryParams
		);
	}
	else
	{
		HitResult = FHitResult();
	}

	return true;
}

void AAutobattlerGrid::ToggleGridVisibility(bool ShouldBeVisible)
{
	SetActorHiddenInGame(!ShouldBeVisible);
	IsCurrentlyHidden = !ShouldBeVisible;
}

void AAutobattlerGrid::ShowOnlyIndicies(const TArray<FIntPair>& IndiciesToShow)
{
	for (auto& IndexInfo : GridIndexInfo)
	{
		if (IndiciesToShow.ContainsByPredicate([IndexInfo](const FIntPair& Index){
			return IndexInfo.GridLocation == Index;
		})) IndexInfo.DecalComponent->SetHiddenInGame(false);
		else IndexInfo.DecalComponent->SetHiddenInGame(true);
	}
}

void AAutobattlerGrid::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAutobattlerGrid, GridIndexInfo);
}
