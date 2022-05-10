// Copyright Robert Zygmunt Uszynski 2021-2022

/* Class header. */
#include "Actors/RandomDecalGenerator.h"

/* Other includes */
#include "Components/ArrowComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialInterface.h"

ARandomDecalGenerator::ARandomDecalGenerator()
{
	/* Component creation */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);

 	/* Reasonable defaults. */
	Bounds = FVector(100.0f);
	DecalSpawnAxis = EDecalSpawnAxis::NegativeZ;
	MaxSpawnAngle = 40.0f;
	TraceChannel = TEnumAsByte<ECollisionChannel>(ECollisionChannel::ECC_Visibility);
	bAlignToNormal = true;
	bUseRandomSeed = true;
	Seed = 12345;
	NumberDecalsToSpawn = 5;
	BoxColor = FColor::Green;
	LineThickness = 1.0f;
}

void ARandomDecalGenerator::SpawnDecals()
{
	if (NumberDecalsToSpawn < 1)
	{
		// Error message.
		return;
	}

	if (DecalTemplates.Num() < 1)
	{
		// Error message.
		return;
	}

	/* Generate new seed. */
	StreamSeed.Initialize(bUseRandomSeed ? Seed : FMath::Rand());

	/* Clear previous decals. */
	for (int32 i = 0; i < DecalComponents.Num(); i++)
	{
		DecalComponents[i]->DestroyComponent();
		DecalComponents[i] = nullptr;
	}

	DecalComponents.Empty(NumberDecalsToSpawn);

	/* Create new decal components. */
	for (int32 i = 0; i < NumberDecalsToSpawn; i++)
	{
		UDecalComponent* NewDecalComponent = NewObject<UDecalComponent>(this, UDecalComponent::StaticClass(), NAME_None);
		NewDecalComponent->SetupAttachment(RootComponent);
		NewDecalComponent->RegisterComponent();
		DecalComponents.Emplace(NewDecalComponent);
	}

	/* Setup collision query params to use later. */
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActors(ActorsToIgnoreInTrace);
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bTraceComplex = bTraceComplex;

	for (int32 i = 0; i < DecalComponents.Num(); i++)
	{
		/* Pick a random decal component and random decal template, then assign that random decal to the component. */
		// int32 RandomIndex = bUseRandomSeed ? StreamSeed.RandRange(0, DecalComponents.Num() - 1) : FMath::RandRange(0, DecalComponents.Num() - 1);
		int32 RandomDecalIndex = bUseRandomSeed ? StreamSeed.RandRange(0, DecalTemplates.Num() - 1) : FMath::RandRange(0, DecalTemplates.Num() - 1);

		UDecalComponent* SelectedComponent = DecalComponents[i];
		UMaterialInterface* DecalMaterial = DecalTemplates[RandomDecalIndex];

		SelectedComponent->SetDecalMaterial(DecalMaterial);

		/* Generate a random decal size. */
		FVector NewDecalSize = FVector::ZeroVector;
		NewDecalSize.X = bRandomiseXSize ? XSizeRange.GenerateRandomNumberInRange(StreamSeed, bUseRandomSeed) : 128.0f;
		NewDecalSize.Y = bRandomiseYSize ? YSizeRange.GenerateRandomNumberInRange(StreamSeed, bUseRandomSeed) : 256.0f;
		NewDecalSize.Z = bRandomiseZSize ? ZSizeRange.GenerateRandomNumberInRange(StreamSeed, bUseRandomSeed) : 256.0f;
		SelectedComponent->DecalSize = NewDecalSize;

		AlignDecalComponent(SelectedComponent, CollisionQueryParams);
	}
}

void ARandomDecalGenerator::OnConstruction(const FTransform& Transform)
{
	switch (DecalSpawnAxis)
	{
		case EDecalSpawnAxis::NegativeX:
		Arrow->SetWorldRotation(FRotator(0.0f, 180.0f, 0.0f));
		break;

		case EDecalSpawnAxis::NegativeY:
		Arrow->SetWorldRotation(FRotator(0.0f, 270.0f, 0.0f));
		break;

		case EDecalSpawnAxis::NegativeZ:
		Arrow->SetWorldRotation(FRotator(270.0f, 0.0f, 0.0f));
		break;

		case EDecalSpawnAxis::PositiveX:
		Arrow->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
		break;

		case EDecalSpawnAxis::PositiveY:
		Arrow->SetWorldRotation(FRotator(0.0f, 90.0f, 0.0f));
		break;

		case EDecalSpawnAxis::PositiveZ:
		Arrow->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));
		break;
	}

	FlushPersistentDebugLines(GetWorld());
	DrawDebugBox(GetWorld(), GetActorLocation(), Bounds, BoxColor, true, -1.0f, 0, LineThickness);

	if (bRefresh)
	{
		SpawnDecals();
		bRefresh = false;
	}
}

FRotator ARandomDecalGenerator::GetDecalRotationFromDecalSpawnAxis(EDecalSpawnAxis NewDecalSpawnAxis)
{
	switch (NewDecalSpawnAxis)
	{
		case EDecalSpawnAxis::NegativeX:
		return(FRotator(0.0f, 180.0f, 0.0f));	

		case EDecalSpawnAxis::NegativeY:
		return(FRotator(0.0f, 270.0f, 0.0f));	

		case EDecalSpawnAxis::NegativeZ:
		return(FRotator(270.0f, 0.0f, 0.0f));

		case EDecalSpawnAxis::PositiveX:
		return(FRotator(0.0f, 0.0f, 0.0f));

		case EDecalSpawnAxis::PositiveY:
		return(FRotator(0.0f, 90.0f, 0.0f));

		case EDecalSpawnAxis::PositiveZ:
		return(FRotator(90.0f, 0.0f, 0.0f));
	}
	
	return FRotator();
}

void ARandomDecalGenerator::AlignDecalComponent(class UDecalComponent* ComponentToModify, const FCollisionQueryParams& CollisionQueryParams)
{
	if (!IsValid(GetWorld()) || !IsValid(ComponentToModify)) return;

	FVector StartTrace;
	FVector EndTrace;
	GetTraceStartEndLocations(RandomPointInBounds(), StartTrace, EndTrace);

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, TraceChannel.GetValue(), CollisionQueryParams);

	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, 10.0f);

	if (!bHit)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString("Failed to find valid surface to move decal to!"));
		return;
	}

	ComponentToModify->SetWorldLocation(HitResult.ImpactPoint);
	
	// FQuat ImpactQuat = HitResult.ImpactNormal.ToOrientationQuat();
	// FMatrix Matrix(ImpactQuat.RotateVector(GetActorForwardVector().GetSafeNormal()), ImpactQuat.RotateVector(GetActorRightVector().GetSafeNormal()), ImpactQuat.RotateVector(GetActorUpVector().GetSafeNormal()), FVector::ZeroVector);

	ComponentToModify->SetWorldRotation(FRotationMatrix::MakeFromZX(HitResult.ImpactNormal, GetActorForwardVector()).Rotator());
	ComponentToModify->AddLocalRotation(FRotator(-90.0f, 0.0f, 0.0f));

	FRotator RelativeRotator;
	RelativeRotator.Yaw = bRandomiseYaw ? YawRange.GenerateRandomNumberInRange(StreamSeed, bUseRandomSeed) : 0.0f;
	RelativeRotator.Pitch = bRandomisePitch ? PitchRange.GenerateRandomNumberInRange(StreamSeed, bUseRandomSeed) : 0.0f;
	RelativeRotator.Roll = bRandomiseRoll ? RollRange.GenerateRandomNumberInRange(StreamSeed, bUseRandomSeed) : 0.0f;

	ComponentToModify->AddLocalRotation(RelativeRotator);
}

float ARandomDecalGenerator::RandomPointInRangeFromStreamSeed(float Min, float Max)
{
	return Min + (Max - Min) * StreamSeed.FRand();
}

FVector ARandomDecalGenerator::RandomPointInBounds()
{
	if (bUseRandomSeed)
	{
		/* To get a pseudo-random point, first we need the Max BoxExtent (which is just the BoxExtent) and its inverse, which is BoxExtent * -1.0f.
		We pick a random point between the Xs, Ys and Zs of these two points from stream, then offset it by the actor location. */
		FVector InverseExtent = Bounds * -1.0f;
		FVector RandPointStream = FVector(RandomPointInRangeFromStreamSeed(InverseExtent.X, Bounds.X), 
		RandomPointInRangeFromStreamSeed(InverseExtent.Y, Bounds.Y), RandomPointInRangeFromStreamSeed(InverseExtent.Z, Bounds.Z));
		return RandPointStream + GetActorLocation();
	}
	/* True randomness just gets a completely random point in a FBox. */
	else return FMath::RandPointInBox(FBox(GetActorLocation() - Bounds, GetActorLocation() + Bounds));
}

void ARandomDecalGenerator::GetTraceStartEndLocations(const FVector& RandomPoint, FVector& StartTraceLocation, FVector& EndTraceLocation)
{
	float X = RandomPoint.X;
	float Y = RandomPoint.Y;
	float Z = RandomPoint.Z;

	switch (DecalSpawnAxis)
	{
		/* Trace from bottom to top. */
		case EDecalSpawnAxis::PositiveX:
		StartTraceLocation = FVector(FMath::Abs(Bounds.X) * -1.0f + GetActorLocation().X, Y, Z);
		EndTraceLocation = FVector(FMath::Abs(Bounds.X) + GetActorLocation().X, Y, Z);
		return;

		/* Trace from top to bottom. */
		case EDecalSpawnAxis::NegativeX:
		StartTraceLocation = FVector(FMath::Abs(Bounds.X) + GetActorLocation().X, Y, Z);
		EndTraceLocation = FVector(FMath::Abs(Bounds.X) * -1.0f + GetActorLocation().X, Y, Z);
		return;

		/* Trace from left to right. */
		case EDecalSpawnAxis::PositiveY:
		StartTraceLocation = FVector(X, FMath::Abs(Bounds.Y) * -1.0f + GetActorLocation().Y, Z);
		EndTraceLocation = FVector(X, FMath::Abs(Bounds.Y) + GetActorLocation().Y, Z);
		return;

		/* Trace from right to left. */
		case EDecalSpawnAxis::NegativeY:
		StartTraceLocation = FVector(X, FMath::Abs(Bounds.Y) + GetActorLocation().Y, Z);
		EndTraceLocation = FVector(X, FMath::Abs(Bounds.Y) * -1.0f + GetActorLocation().Y, Z);
		return;

		/* Trace from down to up. */
		case EDecalSpawnAxis::PositiveZ:
		StartTraceLocation = FVector(X, Y, FMath::Abs(Bounds.Z) * -1.0f + GetActorLocation().Z);
		EndTraceLocation = FVector(X, Y, FMath::Abs(Bounds.Z) + GetActorLocation().Z);
		return;

		/* Trace from up to down. */
		case EDecalSpawnAxis::NegativeZ:
		StartTraceLocation = FVector(X, Y, FMath::Abs(Bounds.Z) + GetActorLocation().Z);
		EndTraceLocation = FVector(X, Y, FMath::Abs(Bounds.Z) * -1.0f + GetActorLocation().Z);
		return;
	}
}
