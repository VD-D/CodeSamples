// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Units/AutobattlerPreviewCharacter.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"
#include "Core/AutobattlerSettings.h"
#include "Core/AutobattlerManager.h"
#include "DataAssets/AutobattlerConfiguration.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "UI/HUD/AutobattlerHUD.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"
#include "Materials/MaterialInterface.h"
#include "Net/UnrealNetwork.h"

AAutobattlerPreviewCharacter::AAutobattlerPreviewCharacter()
{
 	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(RootComponent);
	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	CharacterMesh->SetReceivesDecals(false);

	CharacterDeploymentBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("CharacterDeploymentBounds"));
	CharacterDeploymentBounds->SetupAttachment(RootComponent);
	CharacterDeploymentBounds->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

void AAutobattlerPreviewCharacter::ConfigurePreviewCharacter(const FVector& DeploymentOffset, int32 ID, int32 BudgetCost)
{
	if (!HasAuthority()) return;

	PreviewCharacterCost = BudgetCost;
	PlacementOffset = DeploymentOffset;
	PreviewCharacterID = ID;

	EEntity WhoOwns = EEntity::AI;

	if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
	{
		if (Manager->GetWhoOwnsByID(PreviewCharacterID, WhoOwns))
		{
			if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByID(this, WhoOwns))
			{
				ControllerComponent->OnMouseToGridLocationChanged.AddDynamic(this, &AAutobattlerPreviewCharacter::OnGridLocationChanged);
			}
			else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [BuildPreviewCharacter] Could not get controller component for owner %d!"), *GetName(), (int32)WhoOwns));
		}
		else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [BuildPreviewCharacter] There is no owner for character ID: %d!"), *GetName(), PreviewCharacterID));
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [BuildPreviewCharacter] Could not get manager when configuring preview character!"), *GetName()));
}

void AAutobattlerPreviewCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdatePrimitiveComponentMaterials(false);

	if (APlayerController* LocalController = UAutobattlerControllerComponent::GetLocalPlayerController(this))
	{
		if (UAutobattlerControllerComponent* AutobattlerComponent = UAutobattlerFunctionLibrary::GetFirstComponent<UAutobattlerControllerComponent>(LocalController))
		{
			CanBePlacedReason.AddDynamic(AutobattlerComponent, &UAutobattlerControllerComponent::PlacementReasonChange);
		}
	}
}

void AAutobattlerPreviewCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAutobattlerPreviewCharacter, CanBePlaced);
	DOREPLIFETIME(AAutobattlerPreviewCharacter, ShouldBeVisible);
	DOREPLIFETIME(AAutobattlerPreviewCharacter, PreviewCharacterCost);
	DOREPLIFETIME(AAutobattlerPreviewCharacter, FinalPlacementLocation);
}

void AAutobattlerPreviewCharacter::OnRep_CanBePlaced()
{
	UpdatePrimitiveComponentMaterials(CanBePlaced);
}

void AAutobattlerPreviewCharacter::OnRep_ShouldBeVisible()
{
	ChangeSceneComponentVisibility(ShouldBeVisible);
}

void AAutobattlerPreviewCharacter::OnRep_FinalPlacementLocation()
{
	SetActorLocation(FinalPlacementLocation);
}

void AAutobattlerPreviewCharacter::OnCanBePlaceStateChanged_Implementation(bool NewCanBePlaced)
{
	// TODO: Implement.
}

void AAutobattlerPreviewCharacter::OnGridLocationChanged(EEntity WhoChanged, bool ValidLocation, const FVector& NewLocation, const FIntPair& NewGridIndex)
{
	if (!HasAuthority()) return;

	AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this);
	if (!IsValid(Manager))
	{
		CanBePlaced = false;
		ShouldBeVisible = false;
		ChangeSceneComponentVisibility(ShouldBeVisible);
		ShowPendingBudgetCost(WhoChanged, false, 0);
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [OnGridLocationChanged] Could not get autobattler manager!"), *GetName()));
		return;
	}

	FinalPlacementLocation = NewLocation + PlacementOffset;
	SetActorLocation(FinalPlacementLocation);

	if (!ValidLocation)
	{
		ShouldBeVisible = false;
		ChangeSceneComponentVisibility(ShouldBeVisible);
		ShowPendingBudgetCost(WhoChanged, false, 0);
	}
	else
	{
		ShouldBeVisible = true;
		ChangeSceneComponentVisibility(ShouldBeVisible);
		ShowPendingBudgetCost(WhoChanged, true, PreviewCharacterCost);

		if (Manager->GetCurrentBudgetForEntity(WhoChanged) + PreviewCharacterCost > Manager->GetMaxBudgetForEntity(WhoChanged))
		{
			CanBePlaced = false;
			UpdatePrimitiveComponentMaterials(CanBePlaced);
			BroadcastCannotBePlaced(WhoChanged, EPlaceReason::OverBudget);
			return;
		}

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Emplace(this);

		if (Manager->GetIsBoundsOccupied(
			CharacterDeploymentBounds->GetComponentLocation(),
			CharacterDeploymentBounds->GetComponentRotation(),
			CharacterDeploymentBounds->GetScaledBoxExtent(),
			IsValid(Manager->GetAutobattlerConfigurationAsset()) ? Manager->GetAutobattlerConfigurationAsset()->GridOccupancyCollisionChannel : ECollisionChannel::ECC_Visibility,
			IgnoreActors
			))
		{
			CanBePlaced = false;
			UpdatePrimitiveComponentMaterials(CanBePlaced);
			BroadcastCannotBePlaced(WhoChanged, EPlaceReason::CannotPlaceOnIndex);
			return;
		}

		if (Manager->GetCanPlayerDeployOnIndex(WhoChanged, NewGridIndex))
		{
			CanBePlaced = true;
			UpdatePrimitiveComponentMaterials(CanBePlaced);
			BroadcastCannotBePlaced(WhoChanged, EPlaceReason::CanBePlaced);
		}
		else
		{
			CanBePlaced = false;
			UpdatePrimitiveComponentMaterials(CanBePlaced);
			BroadcastCannotBePlaced(WhoChanged, EPlaceReason::InvalidLocation);
		}
	}
}

void AAutobattlerPreviewCharacter::ShowPendingBudgetCost_Implementation(EEntity WhoOwns, bool NewShow, int32 Cost)
{
	if (APlayerController* LocalController = UAutobattlerControllerComponent::GetLocalPlayerController(this))
	{
		if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerFunctionLibrary::GetFirstComponent<UAutobattlerControllerComponent>(LocalController))
		{
			if (ControllerComponent->GetIdentity() == WhoOwns)
			{
				ControllerComponent->RequestPendingBudgetUIVisibilityChange(NewShow);
				ControllerComponent->RequestBudgetUIUpdate(EBudgetUIType::Pending, Cost);
			}
		}
	}
}

void AAutobattlerPreviewCharacter::BroadcastCannotBePlaced_Implementation(EEntity WhoOwns, EPlaceReason PlacementReason)
{
	CanBePlacedReason.Broadcast(WhoOwns, PlacementReason);
}

void AAutobattlerPreviewCharacter::ChangeSceneComponentVisibility(bool NewVisible)
{
	TArray<USceneComponent*> Components;
	GetComponents<USceneComponent>(Components);

	for (auto SceneComponent : Components)
	{
		if (IsValid(SceneComponent)) SceneComponent->SetHiddenInGame(!NewVisible);
	}
}

void AAutobattlerPreviewCharacter::UpdatePrimitiveComponentMaterials(bool CanPlace)
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);

	for (auto PrimitiveComponent : Components)
	{
		if (!IsValid(PrimitiveComponent)) continue;

		for (int32 i = 0; i < PrimitiveComponent->GetNumMaterials(); i++)
		{
			PrimitiveComponent->SetMaterial(i, CanPlace ? CanPlaceMaterial : CannotPlaceMaterial);
		}
	}
}
