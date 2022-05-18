// Copyright Robert Zygmunt Uszynski 2021-2022

/* Class header */
#include "Actors/InGameInspectable.h"

/* Other includes */
#include "Actors/InspectableCapture.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h"
#include "GameFramework/SpringArmComponent.h"

AInGameInspectable::AInGameInspectable()
{
	/* Component creation. */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	/* Reasonable defaults. */
	bTryAutoFindTargetCapture = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AInGameInspectable::BeginInspection(const FInspectableProperties& InspectableProperties)
{
	/* Error handling: do not allow inspection if world or TargetCapture are invalid. */
	if (!IsValid(GetWorld()))
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s : Trying to spawn in invalid world!"), *GetDebugName(this)));
		return;
	}

	if (!IsValid(TargetCapture))
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s : Target Capture is null or pending kill!"), *GetDebugName(this)));
		return;
	}

	/* Remove previous actors from show list. */
	TargetCapture->DestroyReferencedActors();
	TargetCapture->SpringArm->TargetArmLength = InspectableProperties.TargetCaptureArmLength;

	/* Set template to this in SpawnParameters to create identical clone. */
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	/* Compute world transform where clone is meant to be spawned. */
	FTransform WorldTransform = TargetCapture->GetActorTransform();
	WorldTransform.AddToTranslation(InspectableProperties.TargetCaptureRelativeTransform.GetLocation());
	WorldTransform.ConcatenateRotation(InspectableProperties.TargetCaptureRelativeTransform.GetRotation());
	WorldTransform.SetScale3D(InspectableProperties.TargetCaptureRelativeTransform.GetScale3D());

	/* Actually spawn the replicated inspectable actor (the clone). */
	ReplicatedInspectable = GetWorld()->SpawnActor<AInGameInspectable>(GetClass(), WorldTransform, SpawnParameters);
	if (!IsValid(ReplicatedInspectable))
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s : Failed to spawn InGameInspectable clone!"), *GetDebugName(this)));
		return;
	}

	/* Ensure the replicated inspectable is in the show actor list of both captures. */
	TargetCapture->FinalColorSceneCapture->ShowOnlyActors.Add(ReplicatedInspectable);
	TargetCapture->OpacitySceneCapture->ShowOnlyActors.Add(ReplicatedInspectable);
	TargetCapture->CaptureBegin(this, InspectableProperties.InspectableText);
}

void AInGameInspectable::BeginInspectionCustomDatatable(class UDataTable* CustomDatatable, const FName& CustomDatatableRowName)
{
	if (CustomDatatable == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s : Custom Datatable is invalid!"), *GetDebugName(this)));
		return;
	}

	FInspectableProperties* NewProperties = CustomDatatable->FindRow<FInspectableProperties>(CustomDatatableRowName, FString("In Game Inspectable"));
	if (NewProperties == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s : Custom Datatable : cannot find row %s!"), *GetDebugName(this), *CustomDatatableRowName.ToString()));
		return;
	}

	BeginInspection(*NewProperties);
}

void AInGameInspectable::BeginInspectionDefaultDatatable()
{
	if (DefaultDatatable == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s : Default Datatable is invalid!"), *GetDebugName(this)));
		return;
	}

	FInspectableProperties* NewProperties = DefaultDatatable->FindRow<FInspectableProperties>(DefaultRowName, FString("In Game Inspectable"));
	if (NewProperties == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s : Default Datatable : cannot find row %s!"), *GetDebugName(this), *DefaultRowName.ToString()));
		return;
	}

	BeginInspection(*NewProperties);
}

void AInGameInspectable::InspectableAddRoll(float DeltaRoll)
{
	if (!IsValid(ReplicatedInspectable)) return;

	ReplicatedInspectable->AddActorLocalRotation(FRotator(0.0f, 0.0f, DeltaRoll));
}

void AInGameInspectable::InspectableAddPitch(float DeltaPitch)
{
	if (!IsValid(ReplicatedInspectable)) return;

	ReplicatedInspectable->AddActorLocalRotation(FRotator(DeltaPitch, 0.0f, 0.0f));
}

void AInGameInspectable::InspectableAddYaw(float DeltaYaw)
{
	if (!IsValid(ReplicatedInspectable)) return;

	ReplicatedInspectable->AddActorLocalRotation(FRotator(0.0f, DeltaYaw, 0.0f));
}

void AInGameInspectable::BeginPlay()
{
	Super::BeginPlay();

	/* Set TargetCapture to first valid AInspectableCapture in world if trying to auto find. */
	if (bTryAutoFindTargetCapture && TargetCapture == nullptr)
	{
		for (TActorIterator<AInspectableCapture> Iterator(GetWorld()); Iterator; ++Iterator)
		{
			AInspectableCapture* CurrentCapture = *Iterator;
			if (IsValid(CurrentCapture))
			{
				TargetCapture = CurrentCapture;
				break;
			}
		}
	}
}
