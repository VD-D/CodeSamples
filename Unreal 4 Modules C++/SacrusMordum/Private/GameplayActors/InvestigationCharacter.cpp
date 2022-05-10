// Copyright Robert Zygmunt Uszynski 2020-2021

#include "GameplayActors/InvestigationCharacter.h"
#include "GameplayActors/InvestigationObject.h"
#include "GameplayActors/SacrusMordumWidgetManager.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/InvestigationDiscoveryInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/FocusWidget.h"
#include "../../SacrusMordum.h"

AInvestigationCharacter::AInvestigationCharacter()
{
	/* Camera creation and default values. */
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
	Camera->SetupAttachment(RootComponent);

	/* Investigation "trace" (actually a capsule collision) creating and default values. */
	InvestigationTrace = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Investigation Trace"));
	InvestigationTrace->SetupAttachment(Camera);

	/* Tick not required. */
	PrimaryActorTick.bCanEverTick = false;
}

void AInvestigationCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	/* Bind functions to overlap begin/end for the investigation trace. */
	InvestigationTrace->OnComponentBeginOverlap.AddDynamic(this, &AInvestigationCharacter::InvestigationTraceBeginOverlap);
	InvestigationTrace->OnComponentEndOverlap.AddDynamic(this, &AInvestigationCharacter::InvestigationTraceEndOverlap);

	/* Set the world widget manager as the first one found, if it was not set in-editor. */
	if (WorldWidgetManager == nullptr)
	{
		TArray<AActor*> FoundManagers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASacrusMordumWidgetManager::StaticClass(), FoundManagers);
		if (FoundManagers.Num() > 0)
		{
			WorldWidgetManager = Cast<ASacrusMordumWidgetManager>(FoundManagers[0]);
			UE_LOG(LogSacrusMordum, Warning, TEXT("Investigation Character: World Widget Manager not set, will set one automatically (please set in editor)."))
		}
		else
		{
			UE_LOG(LogSacrusMordum, Warning, TEXT("Investigation Character: No world World Widget Manager in world! Please add one."))
		}
	}
}

void AInvestigationCharacter::InvestigationTraceBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/* Firstly, check if the overlapped actor is indeed an AInvestigationObject actor. */
	AInvestigationObject* NewInvestigationObject = Cast<AInvestigationObject>(OtherActor);

	/* If the cast fails, that means nothing of interest is found. Interrupt the current focus if one is occuring. */
	if (NewInvestigationObject == nullptr)
	{
		UE_LOG(LogSacrusMordum, Log, TEXT("Investigation Trace did not overlap investigation object."))
		bIsSwitchingFocus = false;
		OnFocusInterrupted();
		return;
	}

	/* If the investigation object is valid (as checked above), get its InvestigationObjectProperties struct. */
	FInvestigationObjectProperties NewInvestigationObjectProperties = NewInvestigationObject->GetInvestigationObjectInformation();

	/* Case 1: if the current investigation object is null, we have encountered an investigation object for the first time. 
	Save the pointer to the new investigation object, and begin focus. */
	if (CurrentInvestigationObject == nullptr)
	{
		UE_LOG(LogSacrusMordum, Log, TEXT("Investigation Trace overlapped investigation object: %s."), *NewInvestigationObjectProperties.ObjectName.BaseString)
		CurrentInvestigationObject = NewInvestigationObject;
		bIsSwitchingFocus = false;
		OnFocusBegin();
	}
	/* Case 2: The most complicated case, when switching to focus on a new investigation object. When switching between one and
	the other, interrupt and restart any focus already happening. */
	else if (NewInvestigationObject != CurrentInvestigationObject)
	{
		UE_LOG(LogSacrusMordum, Log, TEXT("Investigation Trace switched overlapping investigation object: %s."), *NewInvestigationObjectProperties.ObjectName.BaseString)
		CurrentInvestigationObject = NewInvestigationObject;
		bIsSwitchingFocus = true;
		OnFocusInterrupted();
		OnFocusBegin();
	}
	/* Case 3: Occurs when the player begins focusing on an object, looks away from it, then looks back to it. Simply restart the focus. */
	else if (NewInvestigationObject == CurrentInvestigationObject)
	{
		UE_LOG(LogSacrusMordum, Log, TEXT("Investigation Trace returned overlapping investigation object: %s."), *NewInvestigationObjectProperties.ObjectName.BaseString)
		bIsSwitchingFocus = false;
		OnFocusBegin();
	}

	/* Finally, execute the InvestigationDiscoveyInterface on all listeners. */
	ExecuteInvestigationDiscoveryInterface(EInvestigationDiscoveryInterfaceType::Found);
}

void AInvestigationCharacter::InvestigationTraceEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	/* Perform a check to see if the investigation trace is overlapping any other objects. If Case 2 of InvestigationTraceBeginOverlap,
	it may be the case that two investigation objects are located next to each other. Therefore, focus is only interrupted if focus is 
	not switching and there are no other investigation objects overlapping the InvestigationTrace. */
	TArray<AActor*> OverlappingInvestigationObjects;
	TSubclassOf<AInvestigationObject> InvestigationObjectType;
	InvestigationTrace->GetOverlappingActors(OverlappingInvestigationObjects, InvestigationObjectType);
	
	if (!bIsSwitchingFocus || OverlappingInvestigationObjects.Num() == 0)
	{
		OnFocusInterrupted();
		ExecuteInvestigationDiscoveryInterface(EInvestigationDiscoveryInterfaceType::Lost);
	}
}

void AInvestigationCharacter::OnFocusBegin()
{
	/* Tell the widget manager to begin focus. If it is invalid, log a warning. */
	if (IsValid(WorldWidgetManager))
	{
		WorldWidgetManager->FocusBegin();
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Investigation Character : When beginning to focus, no world widget manager found (maybe make parent call to Begin Play)"))
	}
}

void AInvestigationCharacter::OnFocusInterrupted()
{
	/* Tell the widget manager to interrupt focus. If it is invalid, log a warning. */
	if (IsValid(WorldWidgetManager))
	{
		WorldWidgetManager->FocusInterrupt();
	}
	else
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Investigation Character : When focus was interrupted, no world widget manager found (maybe make parent call to Begin Play)"))
	}
}

void AInvestigationCharacter::ExecuteInvestigationDiscoveryInterface(EInvestigationDiscoveryInterfaceType InvestigationDiscoveryType)
{
	TArray<AActor*> InvestigationInterfaceListeningActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UInvestigationDiscoveryInterface::StaticClass(), InvestigationInterfaceListeningActors);

	for (auto InvestigationInterfaceListeningActor : InvestigationInterfaceListeningActors)
	{
		IInvestigationDiscoveryInterface* I = Cast<IInvestigationDiscoveryInterface>(InvestigationInterfaceListeningActor);

		if (I == nullptr) break;

		switch (InvestigationDiscoveryType)
		{
		case EInvestigationDiscoveryInterfaceType::Found:
			I->Execute_OnInvestigationObjectFound(InvestigationInterfaceListeningActor, CurrentInvestigationObject);
			break;

		case EInvestigationDiscoveryInterfaceType::Lost:
			I->Execute_OnInvestigationObjectLost(InvestigationInterfaceListeningActor, CurrentInvestigationObject);
			break;
		}
	}

	TArray<UUserWidget*> InvestigationInterfaceListeningWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld(), InvestigationInterfaceListeningWidgets, UInvestigationDiscoveryInterface::StaticClass(), false);

	for (auto InvestigationInterfaceListeningWidget : InvestigationInterfaceListeningWidgets)
	{
		IInvestigationDiscoveryInterface* I = Cast<IInvestigationDiscoveryInterface>(InvestigationInterfaceListeningWidget);

		if (I == nullptr) break;

		switch (InvestigationDiscoveryType)
		{
		case EInvestigationDiscoveryInterfaceType::Found:
			I->Execute_OnInvestigationObjectFound(InvestigationInterfaceListeningWidget, CurrentInvestigationObject);
			break;

		case EInvestigationDiscoveryInterfaceType::Lost:
			I->Execute_OnInvestigationObjectLost(InvestigationInterfaceListeningWidget, CurrentInvestigationObject);
			break;
		}
	}
}
