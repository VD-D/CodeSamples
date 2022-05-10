// Copyright Robert Zygmunt Uszynski 2020-2021

#include "GameplayActors/SacrusMordumWidgetManager.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/BillboardComponent.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/FocusInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/FocusWidget.h"
#include "Widgets/InvestigationInfoWidget.h"
#include "../../SacrusMordum.h"

ASacrusMordumWidgetManager::ASacrusMordumWidgetManager()
{
	/* Billboard component creation. */
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	RootComponent = Billboard;

	/* Setup of focus timeline. */
	FocusTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Focus Timeline"));
	FocusTimeline->SetLooping(false);

 	/* Tick must be enabled for timeline components to work. */
	PrimaryActorTick.bCanEverTick = true;
	/* By default, build focus widget on begin play. */
	bBuildFocusWidgetOnBeginPlay = true;
	/* Set defaults for investigation info widget. */
	bBuildInvestigationInfoWidgetOnBeginPlay = true;
	InvestigationInfoWidgetZOrder = 1;
}

void ASacrusMordumWidgetManager::BuildFocusWidget()
{
	/* Do nothing if the current focus widget is not null. */
	if (CurrentFocusWidget != nullptr) return;

	/* Declare delegates and bind them to timeline callback and end. */
	FOnTimelineFloat FocusTimelineCallbackDelegate;
	FOnTimelineEventStatic FocusTimelineEndDelegate;

	FocusTimelineCallbackDelegate.BindUFunction(this, "WhileFocusing");
	FocusTimelineEndDelegate.BindUFunction(this, "OnFocusCompleted");

	/* Only setup timeline if the Focus Timeline curve is not null. Log warning if it is. */
	if (FocusTimelineCurve != nullptr)
	{
		float Max = 0.0f;
		float Min = 0.0f;
		FocusTimelineCurve->GetTimeRange(Min, Max);

		FocusTimeline->SetTimelineLength(Max);
		FocusTimeline->AddInterpFloat(FocusTimelineCurve, FocusTimelineCallbackDelegate);
		FocusTimeline->SetTimelineFinishedFunc(FocusTimelineEndDelegate);
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: Focus Timeline Curve is null!"))
	}

	/* Create the player's focus widget only if they are spawned in a world. If the widget cannot be created,
	e.g. because the class is null, log a warning. */
	if (GetWorld() != nullptr)
	{
		CurrentFocusWidget = CreateWidget<UFocusWidget>(GetWorld()->GetGameInstance(), FocusWidgetClass);

		if (CurrentFocusWidget != nullptr) CurrentFocusWidget->AddToViewport(FocusWidgetZOrder);
		else
		{
			UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: Focus Widget could not be constructed and is null!"))
		}
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: GetWorld() returns null! Actor likely not spawned in world."))
	}
}

void ASacrusMordumWidgetManager::FocusInterrupt()
{
	/* Check to see if the focus widget is valid. Interrupting the focus sets component visibility to hidden and resets the percentage to 0.
	Log a warning if the focus widget is invalid. */
	if (IsValid(CurrentFocusWidget))
	{
		CurrentFocusWidget->UpdateFocusWidgetComponentVisibility(ESlateVisibility::Hidden);
		CurrentFocusWidget->UpdatePercentage(0.0f);
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: When focus was interrupted, the focus widget is not valid - either null or pending kill (maybe make parent call to Begin Play)"))
	}

	/* Stop timeline upon interrupting. */
	FocusTimeline->Stop();

	/* Execute Focus interface to notify listeners focus has been interrupted. */
	ExecuteFocusInterface(EFocusInterfaceType::FocusInterrupted);
}

void ASacrusMordumWidgetManager::FocusBegin()
{
	/* When beginning to focus, ensure that the current focus widget is valid. Make it visible and set % = 0 if so,
	otherwise log a warning. */
	if (IsValid(CurrentFocusWidget))
	{
		CurrentFocusWidget->UpdateFocusWidgetComponentVisibility(ESlateVisibility::Visible);
		CurrentFocusWidget->UpdatePercentage(0.0f);
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: When beginning to focus, the focus widget is not valid - either null or pending kill (maybe make parent call to Begin Play)"))
	}

	/* Start playing the focus timeline from the start when focusing. */
	FocusTimeline->PlayFromStart();

	/* Execute Focus interface to notify listeners focus has begun. */
	ExecuteFocusInterface(EFocusInterfaceType::FocusBegun);
}

void ASacrusMordumWidgetManager::BuildInvestigationInfoWidget()
{
	if (GetWorld() != nullptr)
	{
		CurrentInvestigationInfoWidget = CreateWidget<UInvestigationInfoWidget>(GetWorld()->GetGameInstance(), InvestigationInfoWidgetClass);
		if (CurrentInvestigationInfoWidget != nullptr) CurrentInvestigationInfoWidget->AddToViewport(InvestigationInfoWidgetZOrder);
		else
		{
			UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: Investigation Info Widget could not be constructed and is null!"))
		}
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: GetWorld() returns null! Actor likely not spawned in world."))
	}
}

void ASacrusMordumWidgetManager::InvestigationInfoWidgetPlayAnim(bool Direction)
{
}

void ASacrusMordumWidgetManager::UpdateInvestigationInfoWidget(FInvestigationObjectProperties NewInfo)
{
	if (IsValid(CurrentInvestigationInfoWidget))
	{
		CurrentInvestigationInfoWidget->UpdateInvestigationInfo(NewInfo);
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: Investigation Info widget is invalid and information cannot be updated."))
	}
}

void ASacrusMordumWidgetManager::UpdateInvestigationInfoScroll(float AxisValue)
{
	if (IsValid(CurrentInvestigationInfoWidget))
	{
		CurrentInvestigationInfoWidget->UpdateScrollBarOffset(AxisValue);
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: Investigation Info widget is invalid and scroll bar offset cannot be updated."))
	}
}

void ASacrusMordumWidgetManager::BeginPlay()
{
	Super::BeginPlay();

	/* Build the widgts if indicated to do so. */
	if (bBuildFocusWidgetOnBeginPlay) BuildFocusWidget();
	if (bBuildInvestigationInfoWidgetOnBeginPlay) BuildInvestigationInfoWidget();
}

void ASacrusMordumWidgetManager::WhileFocusing(float Value)
{
	/* While we're focusing, continue ensuring the focus widget is valid (in case some other call removes or destroys it).
	Update the percentage according to the curve. If not valid, log a warning. */
	if (IsValid(CurrentFocusWidget))
	{
		CurrentFocusWidget->UpdatePercentage(Value);
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Widget Manager: While focusing, the focus widget is not valid - either null or pending kill (maybe make parent call to Begin Play)"))
	}
}

void ASacrusMordumWidgetManager::OnFocusCompleted()
{
	/* When completing focus, tell the focus widget that has occured and also create the info widget about the investigation object.
	If the CurrentInvestigationObject becomes invalid, log a warning. */
	/*
	if (IsValid(CurrentInvestigationObject))
	{
		// CurrentInvestigationObject->CreateInfoWidget();
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Investigation Character : When focus was completed, Current Investigation Object was either null or pending kill!"))
	}
	*/

	/* Execute Focus interface to notify listeners focus has been completed. */
	ExecuteFocusInterface(EFocusInterfaceType::FocusCompleted);
}

void ASacrusMordumWidgetManager::ExecuteFocusInterface(EFocusInterfaceType FocusInterfaceType)
{
	TArray<AActor*> FocusInterfaceListeningActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UFocusInterface::StaticClass(), FocusInterfaceListeningActors);

	for (auto FocusInterfaceListeningActor : FocusInterfaceListeningActors)
	{
		IFocusInterface* I = Cast<IFocusInterface>(FocusInterfaceListeningActor);

		if (I == nullptr) break;

		switch (FocusInterfaceType)
		{
		case EFocusInterfaceType::FocusBegun:
			I->Execute_OnFocusBegan(FocusInterfaceListeningActor);
			break;

		case EFocusInterfaceType::FocusInterrupted:
			I->Execute_OnFocusInterrupted(FocusInterfaceListeningActor);
			break;

		case EFocusInterfaceType::FocusCompleted:
			I->Execute_OnFocusCompleted(FocusInterfaceListeningActor);
			break;
		}
	}

	TArray<UUserWidget*> FocusInterfaceListeningWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld(), FocusInterfaceListeningWidgets, UFocusInterface::StaticClass(), false);

	for (auto FocusInterfaceListeningWidget : FocusInterfaceListeningWidgets)
	{
		IFocusInterface* I = Cast<IFocusInterface>(FocusInterfaceListeningWidget);

		if (I == nullptr) break;

		switch (FocusInterfaceType)
		{
		case EFocusInterfaceType::FocusBegun:
			I->Execute_OnFocusBegan(FocusInterfaceListeningWidget);
			break;

		case EFocusInterfaceType::FocusInterrupted:
			I->Execute_OnFocusInterrupted(FocusInterfaceListeningWidget);
			break;

		case EFocusInterfaceType::FocusCompleted:
			I->Execute_OnFocusCompleted(FocusInterfaceListeningWidget);
			break;
		}
	}
}
