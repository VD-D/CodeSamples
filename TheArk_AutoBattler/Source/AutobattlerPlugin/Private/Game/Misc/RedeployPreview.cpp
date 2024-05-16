// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Misc/RedeployPreview.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"
#include "Core/AutobattlerManager.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Components/DecalComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"

ARedeployPreview::ARedeployPreview()
{
	ActorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ActorRoot"));
	RootComponent = ActorRoot;

	DecalSize = FVector(128.0f);
	AdditionalMeshRotation = FRotator(270.0f, 0.0f, 0.0f);
	ZOffset = 150.0f;
	MeshScale = 0.2f;
	MeshSpacing = 15.0f;
	PrimaryActorTick.bCanEverTick = true;
}

void ARedeployPreview::ConfigureRedployPreview(EEntity WhoOwns)
{
	if (!HasAuthority()) return;

	if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByID(this, WhoOwns))
	{
		if (!ControllerComponent->OnMouseToGridLocationChanged.IsAlreadyBound(this, &ARedeployPreview::MouseToGridChanged))
		{
			ControllerComponent->OnMouseToGridLocationChanged.AddDynamic(this, &ARedeployPreview::MouseToGridChanged);
			OwnerInternal = WhoOwns;
		}
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [ConfigureRedployPreview] Could not find controller component for entity %s"), *GetName(), *UEnum::GetValueAsString(WhoOwns)));
}

void ARedeployPreview::MouseToGridChanged(EEntity WhoChanged, bool bLocationValid, const FVector& NewLocation, const FIntPair& NewGridIndex)
{
	if (!HasAuthority()) return;

	bool CanPlayerDeployOn = false;
	if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
	{
		CanPlayerDeployOn = Manager->GetCanPlayerDeployOnIndex(OwnerInternal, NewGridIndex) && !Manager->GetIsGridIndexOccupied(NewGridIndex);
	}

	Multicast_UpdateVisualisation(
		OwnerInternal, 
		bLocationValid, 
		CanPlayerDeployOn, 
		NewLocation,
		ActorRoot->GetComponentLocation(),
		ZOffset,
		MeshScale,
		MeshSpacing,
		PreviewMesh,
		CanPlayerDeployOn ? CanPlacePreviewMeshMaterials : CannotPlacePreviewMeshMaterials,
		AdditionalMeshRotation
	);
}

void ARedeployPreview::Multicast_UpdateVisualisation_Implementation(
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
)
{
	if (APlayerController* LocalController = UAutobattlerControllerComponent::GetLocalPlayerController(this))
	{
		if (UAutobattlerControllerComponent* ControllerComponent = Cast<UAutobattlerControllerComponent>(LocalController->GetComponentByClass(UAutobattlerControllerComponent::StaticClass())))
		{
			if (ControllerComponent->GetIdentity() == ActualOwner)
			{
				DestroyAllComponentsOfClass<UDecalComponent>();
				DestroyAllComponentsOfClass<USplineComponent>();
				DestroyAllComponentsOfClass<UInstancedStaticMeshComponent>();

				if (bLocationValid)
				{
					FAttachmentTransformRules ComponentAttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

					UDecalComponent* NewDecalComponent = NewObject<UDecalComponent>(this, UDecalComponent::StaticClass());
					NewDecalComponent->RegisterComponent();
					NewDecalComponent->AttachToComponent(RootComponent, ComponentAttachmentTransformRules);
					NewDecalComponent->SetWorldLocation(NewLocation);
					NewDecalComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
					NewDecalComponent->SetMaterial(0, CanDeploy ? CanDeployOnMaterial : CannotDeployOnMaterial);
					NewDecalComponent->DecalSize = DecalSize;

					if (NewPreviewMesh == nullptr) return;

					USplineComponent* NewSplineComponent = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
					NewSplineComponent->RegisterComponent();
					NewSplineComponent->AttachToComponent(RootComponent, ComponentAttachmentTransformRules);
					NewSplineComponent->AddSplinePoint(FVector::ZeroVector, ESplineCoordinateSpace::Local);
					NewSplineComponent->AddSplinePoint(GetMidpointWithOffset(StartLocation, NewLocation, NewZPointOffset), ESplineCoordinateSpace::World);
					NewSplineComponent->AddSplinePoint(NewLocation, ESplineCoordinateSpace::World);

					UInstancedStaticMeshComponent* ISM = NewObject<UInstancedStaticMeshComponent>(this, UInstancedStaticMeshComponent::StaticClass());
					ISM->RegisterComponent();
					ISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					ISM->AttachToComponent(RootComponent, ComponentAttachmentTransformRules);
					ISM->SetStaticMesh(NewPreviewMesh);

					for (int32 i = 0; i < NewPreviewMeshMaterials.Num(); i++)
					{
						ISM->SetMaterial(i, NewPreviewMeshMaterials[i]);
					}
					
					const FBox MeshBoundingBox = NewPreviewMesh->GetBoundingBox(); 
					float SpacingScaled = (MeshBoundingBox.Max - MeshBoundingBox.Min).X * NewMeshTransformScale + NewMeshSpacing;

					int32 NumberOfInstances = FMath::FloorToInt(NewSplineComponent->GetSplineLength() / SpacingScaled);

					for (int32 i = 0; i <= NumberOfInstances; i++)
					{
						FVector InstanceLocation = NewSplineComponent->GetLocationAtDistanceAlongSpline(i * SpacingScaled, ESplineCoordinateSpace::Local);
						FRotator InstanceRotation = NewSplineComponent->GetRotationAtDistanceAlongSpline(i * SpacingScaled, ESplineCoordinateSpace::Local) + NewAdditionalRotation;
						FVector InstanceScale = FVector(NewMeshTransformScale);
						
						ISM->AddInstance(FTransform(InstanceRotation, InstanceLocation, InstanceScale));
					}
				}
			}
		}
	}
}

FVector ARedeployPreview::GetMidpointWithOffset(const FVector& Start, const FVector& End, float MidpointZOffset)
{
	return FVector((Start.X + End.X) / 2.0f, (Start.Y + End.Y) / 2.0f, ((Start.Z + End.Z) / 2.0f) + MidpointZOffset);
}
