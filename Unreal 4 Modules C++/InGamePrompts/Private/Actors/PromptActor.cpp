// Copyright Robert Zygmunt Uszynski 2021-2022

/* Class header */
#include "Actors/PromptActor.h"

/* Other includes. */
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/Prompt.h"

APromptActor::APromptActor()
{
	/* Component creation */
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	PromptProximityBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PromptProximityBox"));
	PromptProximityBox->ShapeColor = FColor::Red;
	PromptProximityBox->SetupAttachment(RootComponent);

	PromptProximitySphere = CreateDefaultSubobject<USphereComponent>(TEXT("PromptProximitySphere"));
	PromptProximitySphere->ShapeColor = FColor::Red;
	PromptProximitySphere->SetupAttachment(RootComponent);
	
	PromptComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromptComponent"));
	PromptComponent->SetupAttachment(RootComponent);
	PromptComponent->SetHiddenInGame(true);
 	
	/* Reasonable defaults */
	InteractKeyActionEventName = FName("Interact");
	PromptProximityType = EPromptProximityType::Box;
	bInitiallyEnabled = true;
	PromptText = FText::GetEmpty();

	PrimaryActorTick.bCanEverTick = false;
}

void APromptActor::SetPromptEnabled(bool NewEnabled)
{
	UPrompt* Prompt = GetPromptWidget();
	if (Prompt == nullptr) return;
	
	bIsPromptEnabled = NewEnabled;

	if (bPlayerInProximityRadius) Prompt->FadeIn();
	else Prompt->FadeOut();
}

void APromptActor::OnConstruction(const FTransform& Transform)
{
	/* When switching proximity type, make the box or sphere visible or invisible, depending on
	selected proximity type. */
	switch (PromptProximityType)
	{
		case EPromptProximityType::Box:
			PromptProximityBox->SetVisibility(true);
			PromptProximitySphere->SetVisibility(false);
			break;

		case EPromptProximityType::Sphere:
			PromptProximityBox->SetVisibility(false);
			PromptProximitySphere->SetVisibility(true);
			break;
	}
}

void APromptActor::BeginPlay()
{
	Super::BeginPlay();

	/* Bind overlap delegates. */
	PromptProximityBox->OnComponentBeginOverlap.AddDynamic(this, &APromptActor::OnOverlapBeginBox);
	PromptProximityBox->OnComponentEndOverlap.AddDynamic(this, &APromptActor::OnOverlapEndBox);
	PromptProximitySphere->OnComponentBeginOverlap.AddDynamic(this, &APromptActor::OnOverlapBeginSphere);
	PromptProximitySphere->OnComponentEndOverlap.AddDynamic(this, &APromptActor::OnOverlapEndSphere);

	/* Configure some prompt settings. */
	PromptComponent->SetWidgetClass(PromptWidgetClass);
	PromptComponent->SetHiddenInGame(true);
	bIsPromptEnabled = bInitiallyEnabled;
}

UPrompt* APromptActor::GetPromptWidget() const
{
	return Cast<UPrompt>(PromptComponent->GetWidget());
}

void APromptActor::OnOverlapBeginBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* Do nothing if proximity type is not of box type. */
	if (PromptProximityType != EPromptProximityType::Box) return;
	InitialisePrompt(OtherActor, true);
}

void APromptActor::OnOverlapBeginSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* Do nothing if proximity type is not of sphere type. */
	if (PromptProximityType != EPromptProximityType::Sphere) return;
	InitialisePrompt(OtherActor, true);
}

void APromptActor::OnOverlapEndBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/* Do nothing if proximity type is not of box type. */
	if (PromptProximityType != EPromptProximityType::Box) return;
	InitialisePrompt(OtherActor, false);
}

void APromptActor::OnOverlapEndSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/* Do nothing if proximity type is not of sphere type. */
	if (PromptProximityType != EPromptProximityType::Sphere) return;
	InitialisePrompt(OtherActor, false);
}

void APromptActor::InitialisePrompt(AActor* OverlappingActor, bool Entered)
{
	/* Test to see if Overlapping actor is pawn, if world is valid,
	if first player controller is valid, if PlayerPawn is valid and if
	the overlapping pawn == player pawn. */
	APawn* PawnToTest = Cast<APawn>(OverlappingActor);
	if (PawnToTest == nullptr) return;

	if (!IsValid(GetWorld())) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC == nullptr) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (PlayerPawn == nullptr || PlayerPawn != PawnToTest) return;

	/* Flag whether or not the player left or entered the radius. */
	bPlayerInProximityRadius = Entered;

	/* Get the prompt. */
	UPrompt* Prompt = GetPromptWidget();
	if (Prompt == nullptr) return;

	/* Ensure prompt is visible in game. */
	PromptComponent->SetHiddenInGame(false);
	Prompt->SetPromptProperties(PromptText, bIsTextOnly, IconCreatorSettings);

	/* Fade the prompt out if the prompt is disabled. Otherwise, fade in or fade out
	depending on whether or not the player is entering or leaving the proximity radius. */
	if (!bIsPromptEnabled) Prompt->FadeOut();
	else
	{
		if (bPlayerInProximityRadius) Prompt->FadeIn();
		else Prompt->FadeOut();
	}
}
