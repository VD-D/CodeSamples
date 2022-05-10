// Copyright Robert Zygmunt Uszynski 2020-2021

#include "InvestigationClipboard.h"

#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"

AInvestigationClipboard::AInvestigationClipboard()
{
	/* Component creation. */
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(SceneRoot);

	PaperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaperMesh"));
	PaperMesh->SetupAttachment(StaticMesh);

	CloseTextCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CloseTextCollision"));
	CloseTextCollisionComponent->SetupAttachment(PaperMesh);

	CloseText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CloseText"));
	CloseText->SetupAttachment(CloseTextCollisionComponent);
	CloseText->Text = FText::FromString(FString("Close (X)"));

	SplineTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SplineTimeline"));
	SplineTimeline->SetLooping(false);

	/* Tick must be enabled for spline to be traversed. */
	PrimaryActorTick.bCanEverTick = true;

	/* Reasonable default for world size of close text. */
	CloseTextMouseOverSize = 5.0f;
	CloseTextMouseOverEndSize = 5.0f;

	/* Reasonable defaults for Page Text configuration. */
	InitialTextLocation = FVector(-34.0f, -33.8f, 1.0f);
	InitialTextRotation = FRotator(90.0f, 90.0f, 0.0f);
	NextTextAdjustLocation = FVector(0.0f, 2.7f, 0.0f);
	MaxTextLength = 40;
	MaxNumberOfLines = 30;
	TextColor = FColor::Black;
	TextXScale = 0.3f;
	TextYScale = 1.0f;
	TextWorldSize = 3.0f;
	// TextVerticalAligment = TEnumAsByte<EVerticalTextAligment>(EVerticalTextAligment::EVRTA_TextCenter);
}

void AInvestigationClipboard::AddTextToPage(FString TextToAdd, int32 Position)
{
	/* If the length of the text to add exceeds the maximum length or the position is less than 0 or the position is greater than
	the maximum number of lines, do nothing. */
	if (TextToAdd.Len() > MaxTextLength)
	{
		// Error message here
		return;
	}

	if (Position < 0 || Position > MaxNumberOfLines)
	{
		// Error message here
		return;
	}

	/* Create the new text render component. If this fails, return nothing. */
	UTextRenderComponent* NewTextRenderComponent = NewObject<UTextRenderComponent>(this);
	if (NewTextRenderComponent == nullptr)
	{
		// Error message here.
		return;
	}

	/* Setup the component attachment by attaching it to the paper mesh. */
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepRelativeTransform;

	NewTextRenderComponent->RegisterComponent();
	NewTextRenderComponent->AttachToComponent(PaperMesh, AttachmentRules);
	NewTextRenderComponent->SetRelativeRotation(InitialTextRotation);
	NewTextRenderComponent->SetRelativeLocation(InitialTextLocation - (NextTextAdjustLocation * (float)Position));

	/* Set parameters on the text itself. */
	NewTextRenderComponent->SetText(TextToAdd);
	NewTextRenderComponent->Font = TextFont;
	NewTextRenderComponent->TextMaterial = TextMaterial;
	NewTextRenderComponent->TextRenderColor = TextColor;
	NewTextRenderComponent->WorldSize = TextWorldSize;
	NewTextRenderComponent->XScale = TextXScale;
	NewTextRenderComponent->YScale = TextYScale;
	NewTextRenderComponent->VerticalAlignment = TextVerticalAligment;
	NewTextRenderComponent->HorizontalAlignment = TextHorizontalAligment;
}

void AInvestigationClipboard::BeginPlay()
{
	/* Super to configure general mouse collision. */
	Super::BeginPlay();

	/* Configure timeline on begin play. */
	ConfigureSplineTimeline();

	/* Configure collision on CloseTextCollision.*/
	CloseTextCollisionComponent->OnBeginCursorOver.AddDynamic(this, &AInvestigationClipboard::CloseCollisionMouseOver);
	CloseTextCollisionComponent->OnClicked.AddDynamic(this, &AInvestigationClipboard::CloseCollisionClicked);
	CloseTextCollisionComponent->OnEndCursorOver.AddDynamic(this, &AInvestigationClipboard::CloseCollisionMouseOverEnd);

	/* Ensure text is of size it will be changed to later. */
	CloseText->SetWorldSize(CloseTextMouseOverEndSize);

	/* Initially disable collision on the close text collision component. */
	CloseTextCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	/* Add initial text to the page. */
	for (auto Line : PageText)
	{
		AddTextToPage(Line.Value, Line.Key);
	}
}

void AInvestigationClipboard::OnMouseClicked_Implementation(UPrimitiveComponent * TouchedComponent)
{
	/* Warn and do nothing if spline timeline was incorrectly configured. */
	if (!bIsSplineTimelineConfigured)
	{
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : On Mouse Clicked: Spline timeline was incorrectly configured!"), *GetDebugName(this)));
		return;
	}

	/* Note previously enabled collision type, then disable collision (this prevents re-clicking the collision bounds). */
	SelectionBoundsCollisionEnabledType = SelectionBounds->GetCollisionEnabled();
	SelectionBounds->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	/* Interpolate along the spline toward the end. */
	SplineTimeline->PlayFromStart();
}

void AInvestigationClipboard::SetComponentRenderCustomDepth(bool RenderInCustomDepth)
{
	/* We only want to render the base static mesh (representing the clipboard) in custom depth. */
	StaticMesh->SetRenderCustomDepth(RenderInCustomDepth);
}

void AInvestigationClipboard::ConfigureSplineTimeline()
{
	if (SplineTraversalCurve != nullptr && SplineRotationCurve != nullptr)
	{
		/* Get length of passed SplineTraversalCurve. */
		float MinA = 0.0f;
		float MaxA = 0.0f;
		SplineTraversalCurve->GetTimeRange(MinA, MaxA);
		
		/* Get length of passed SplineRotationCurve. */
		float MinB = 0.0f;
		float MaxB = 0.0f;
		SplineRotationCurve->GetTimeRange(MinB, MaxB);

		/* Make timeline max length equal to longest curve. */
		float Max = MaxB > MaxA ? MaxB : MaxA;

		/* Declare delegates and bind them to the spline timeline. */
		FOnTimelineFloat OnSplineTimelineCallback;
		FOnTimelineVector OnSplineRotationCallback;
		FOnTimelineEventStatic OnSplineTimelineFinishedCallback;

		OnSplineTimelineCallback.BindUFunction(this, FName("SplineTimelineCallback"));
		OnSplineRotationCallback.BindUFunction(this, FName("SplineRotationCallback"));
		OnSplineTimelineFinishedCallback.BindUFunction(this, FName("SplineTimelineOnEnd"));

		/* Configure timeline based on configured/derived variables. */
		SplineTimeline->SetTimelineLength(Max);
		SplineTimeline->AddInterpFloat(SplineTraversalCurve, OnSplineTimelineCallback);
		SplineTimeline->AddInterpVector(SplineRotationCurve, OnSplineRotationCallback);
		SplineTimeline->SetTimelineFinishedFunc(OnSplineTimelineFinishedCallback);

		/* Flag that the timeline was configured. */
		bIsSplineTimelineConfigured = true;
	}
	else
	{
		/* If the curve is invalid, log and do nothing. */
		if (GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s : No SplineTraversalCurve or SplineRotationCurve provided!"), *GetDebugName(this)));
	}
}

void AInvestigationClipboard::SplineTimelineCallback(float Value)
{
	/* Get the Distance along the spline by getting its length * Value clamped between 0 to 1. E.g. If Value is 0.5 we get the point half-way along the spline. */
	float DistanceAlongSpline = Spline->GetSplineLength() * FMath::Clamp(Value, 0.0f, 1.0f);

	/* Set location and rotation of static mesh based on spline. */
	StaticMesh->SetWorldLocation(Spline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Type::World));
}

void AInvestigationClipboard::SplineRotationCallback(FVector Value)
{
	StaticMesh->SetRelativeRotation(FRotator(Value.Y, Value.Z, Value.X));
}

void AInvestigationClipboard::SplineTimelineOnEnd()
{
	bIsZoomedIn = !bIsZoomedIn;
	if (bIsZoomedIn) CloseTextCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	else SelectionBounds->SetCollisionEnabled(SelectionBoundsCollisionEnabledType);
}

void AInvestigationClipboard::CloseCollisionMouseOver(UPrimitiveComponent * TouchedComponent)
{
	CloseText->SetWorldSize(CloseTextMouseOverSize);
}

void AInvestigationClipboard::CloseCollisionClicked(UPrimitiveComponent * TouchedComponent, FKey PressedKey)
{
	if (bIsZoomedIn)
	{
		SplineTimeline->ReverseFromEnd();
		CloseTextCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void AInvestigationClipboard::CloseCollisionMouseOverEnd(UPrimitiveComponent * TouchedComponent)
{
	CloseText->SetWorldSize(CloseTextMouseOverEndSize);
}
