// Copyright Robert Zygmunt Uszynski 2020-2021

#include "GameplayActors/InvestigationPin.h"
#include "DetectiveRoom/DetectiveRoomPawn.h"

#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/TextDisplayWidget.h"

AInvestigationPin::AInvestigationPin()
{
	/* Creation of components. */
	PinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pin Mesh"));
	PinMesh->SetupAttachment(RootComponent);

	Outline = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Outline"));
	Outline->SetupAttachment(RootComponent);

	InfoDisplay = CreateDefaultSubobject<UWidgetComponent>(TEXT("Info Display"));
	InfoDisplay->SetupAttachment(PinMesh);
	InfoDisplay->SetWidgetSpace(EWidgetSpace::Screen);
	InfoDisplay->SetHiddenInGame(true);
}

void AInvestigationPin::SetShowOutline(bool ShowOutline)
{
	Outline->SetHiddenInGame(!ShowOutline);
}

void AInvestigationPin::OnMouseOver_Implementation(UPrimitiveComponent * TouchedComponent)
{
	/* Firstly, see if the controlled pawn is a detective room pawn. */
	ADetectiveRoomPawn* DetectiveRoomPawn = Cast<ADetectiveRoomPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (DetectiveRoomPawn == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : Could not cast controlled pawn to DetectiveRoomPawn!"), *GetDebugName(this)));
		return;
	}

	/* If it is and it is not looking at the map, do nothing. */
	if (DetectiveRoomPawn->GetCurrentViewTarget() != EDetectiveRoomViewTarget::Map) return;

	/* When mousing over, display the widget. */
	InfoDisplay->SetHiddenInGame(false);

	UTextDisplayWidget* TextDisplay = Cast<UTextDisplayWidget>(InfoDisplay->GetWidget());
	if (TextDisplay == nullptr)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : Info Widget is not of type UTextDisplayWidget!"), *GetDebugName(this)));
		return;
	}
	else
	{
		TextDisplay->DisplayText = DescriptionText;
	}
}

void AInvestigationPin::OnMouseClicked_Implementation(UPrimitiveComponent * TouchedComponent)
{
	/* Setup latent info struct to execute function after the level has been loaded. */
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnInvestigationLevelLoaded");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0;

	/* Load the stream level. */
	UGameplayStatics::LoadStreamLevel(this, StreamLevelName, true, false, LatentInfo);
}

void AInvestigationPin::OnMouseOverEnd_Implementation(UPrimitiveComponent * TouchedComponent)
{
	/* When mouse over ends, hide the widget. */
	InfoDisplay->SetHiddenInGame(true);
}

void AInvestigationPin::OnInvestigationLevelLoaded()
{
}
