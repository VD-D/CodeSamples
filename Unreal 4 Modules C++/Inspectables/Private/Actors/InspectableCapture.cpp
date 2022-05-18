// Copyright Robert Zygmunt Uszynski 2021-2022

/* Class header */
#include "Actors/InspectableCapture.h"

/* Other includes */
#include "Actors/InGameInspectable.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Widgets/InspectableWidget.h"

AInspectableCapture::AInspectableCapture()
{
	/* Component creation. */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 50.0f;
	SpringArm->bDoCollisionTest = false;

	FinalColorSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("FinalColorSceneCapture"));
	FinalColorSceneCapture->SetupAttachment(SpringArm);
	FinalColorSceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	FinalColorSceneCapture->CaptureSource = TEnumAsByte<ESceneCaptureSource>(ESceneCaptureSource::SCS_FinalColorHDR);

	OpacitySceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("OpacitySceneCapture"));
	OpacitySceneCapture->SetupAttachment(SpringArm);
	OpacitySceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	OpacitySceneCapture->CaptureSource = TEnumAsByte<ESceneCaptureSource>(ESceneCaptureSource::SCS_SceneColorHDR);

	PrimaryActorTick.bCanEverTick = true;
}

void AInspectableCapture::DestroyReferencedActors()
{
	OpacitySceneCapture->ShowOnlyActors.Empty();

	for (int32 i = 0; i < FinalColorSceneCapture->ShowOnlyActors.Num(); i++)
	{
		FinalColorSceneCapture->ShowOnlyActors[i]->Destroy();
		FinalColorSceneCapture->ShowOnlyActors[i] = nullptr;
	}

	FinalColorSceneCapture->ShowOnlyActors.Empty();
}

void AInspectableCapture::CaptureBegin(AInGameInspectable* Caller, const FText& PassedText)
{
	if (InspectableWidgetClass.Get() == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s : Specify a InspectableWidget class in InspectableCapture!"), *GetDebugName(this)));
		return;
	}

	if (!IsValid(CurrentInspectableWidget)) CurrentInspectableWidget = CreateWidget<UInspectableWidget>(GetWorld(), InspectableWidgetClass);
	if (!CurrentInspectableWidget->IsInViewport()) CurrentInspectableWidget->AddToViewport(InspectableWidgetZOrder);
	CurrentInspectableWidget->UpdateText(PassedText);
	Caller->InspectionBeganDelegate.Broadcast(CurrentInspectableWidget);
}
