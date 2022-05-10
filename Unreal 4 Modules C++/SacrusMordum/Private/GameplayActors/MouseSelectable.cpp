// Copyright Robert Zygmunt Uszynski 2020-2021

#include "GameplayActors/MouseSelectable.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "GameplayInterfaces/MouseSelectableInterface.h"
#include "Kismet/GameplayStatics.h"

AMouseSelectable::AMouseSelectable()
{
	/* Tick is unnecessary. */
	PrimaryActorTick.bCanEverTick = false;
	
	/* Component Configuration. */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	SelectionBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionBounds"));
	SelectionBounds->SetGenerateOverlapEvents(true);
	SelectionBounds->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SelectionBounds->SetupAttachment(SceneRoot);
}

void AMouseSelectable::BeginPlay()
{
	/* Bind delegates. Done on begin play to prevent conflicts with in-editor changes to subclasses. */
	Super::BeginPlay();
	SelectionBounds->OnBeginCursorOver.AddUniqueDynamic(this, &AMouseSelectable::MouseSelectableMouseOver);
	SelectionBounds->OnClicked.AddDynamic(this, &AMouseSelectable::MouseSelectableClicked);
	SelectionBounds->OnEndCursorOver.AddUniqueDynamic(this, &AMouseSelectable::MouseSelectableMouseOverEnd);
}

void AMouseSelectable::OnMouseOver_Implementation(UPrimitiveComponent * TouchedComponent)
{
	/* Dummy Declaration, exsits to be overwritten. */
}

void AMouseSelectable::OnMouseClicked_Implementation(UPrimitiveComponent * TouchedComponent)
{
	/* Dummy Declaration, exsits to be overwritten. */
}

void AMouseSelectable::OnMouseOverEnd_Implementation(UPrimitiveComponent * TouchedComponent)
{
	/* Dummy Declaration, exsits to be overwritten. */
}

void AMouseSelectable::MouseSelectableMouseOver(UPrimitiveComponent * TouchedComponent)
{
	/* Execute mouse selectable interface and call OnMouseOver. */
	CallMouseSelectableInterfaceOnListeners(EInterfaceFunctionType::MouseOverStart, TouchedComponent);
	OnMouseOver(TouchedComponent);
}

void AMouseSelectable::MouseSelectableClicked(UPrimitiveComponent * TouchedComponent, FKey PressedKey)
{
	/* Execute mouse selectable interface and call OnMouseClicked. */
	CallMouseSelectableInterfaceOnListeners(EInterfaceFunctionType::Clicked, TouchedComponent);
	OnMouseClicked(TouchedComponent);
}

void AMouseSelectable::MouseSelectableMouseOverEnd(UPrimitiveComponent * TouchedComponent)
{
	/* Execute mouse selectable interface and call OnMouseOverEnd. */
	CallMouseSelectableInterfaceOnListeners(EInterfaceFunctionType::MouseOverEnd, TouchedComponent);
	OnMouseOverEnd(TouchedComponent);
}

void AMouseSelectable::CallMouseSelectableInterfaceOnListeners(EInterfaceFunctionType FunctionCallType, UPrimitiveComponent * TouchedComponent)
{
	/* Do nothing if GEngine or World pointers are null (likely only happens if not actually spawned in world). */
	if (GEngine == nullptr || GetWorld() == nullptr) return;

	/* Try to get the world context. This prevents calling interface loaded in editor. */
	UWorld* WorldContext = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (WorldContext == nullptr) return;

	/* Default key to pass to interface. */
	FKey DefaultKey = FKey();

	/* Iterate over all objects. On those which have world context and implemenet IMouseSelectableInterface, execute
	a function based on FunctionCallType. */
	for (TObjectIterator<UObject> Itr; Itr; ++Itr)
	{
		UObject* ListeningObject = *Itr;

		if (ListeningObject == nullptr || ListeningObject->GetWorld() != WorldContext || !ListeningObject->GetClass()->ImplementsInterface(UMouseSelectableInterface::StaticClass())) continue;

		IMouseSelectableInterface* I = Cast<IMouseSelectableInterface>(ListeningObject);
		if (I != nullptr)
		{
			switch (FunctionCallType)
			{
			case EInterfaceFunctionType::MouseOverStart:
				I->Execute_OnMouseSelectableMouseOverStart(ListeningObject, TouchedComponent, MetaInfo);
				break;

			case EInterfaceFunctionType::Clicked:
				I->Execute_OnMouseSelectableMouseClicked(ListeningObject, TouchedComponent, DefaultKey, MetaInfo);
				break;

			case EInterfaceFunctionType::MouseOverEnd:
				I->Execute_OnMouseSelectableMouseOverEnd(ListeningObject, TouchedComponent, MetaInfo);
				break;
			}
		}
	}
}
