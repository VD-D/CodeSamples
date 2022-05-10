// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayActors/MouseOverHighlightable.h"
#include "InvestigationClipboard.generated.h"

/* The investigation clipboard adds the functionality to "zoom" up into the player's face,
and can create and communicate with UClipboardText widget to add more lines of text to paper sheet. */
UCLASS()
class SACRUSMORDUM_API AInvestigationClipboard : public AMouseOverHighlightable
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- COMPONENTS --- */

	/* Spline component for moving the clipboard. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Clipboard")
	class USplineComponent* Spline;

	/* Static mesh component representing the paper. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Clipboard")
	class UStaticMeshComponent* PaperMesh;

	/* Collision which can be clicked on to close the clipboard. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Clipboard")
	class UBoxComponent* CloseTextCollisionComponent;

	/* Text render on the paper which prompts to "close" the clipboard, i.e. put it back. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Clipboard")
	class UTextRenderComponent* CloseText;

	/* --- CONFIGURABLE --- */

	/* Float curve required to run spline traversal. Values must be between 0 to 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Curves")
	class UCurveFloat* SplineTraversalCurve;

	/* Float curve required to run spline rotation. Values must be between -360 to 360. Note that rotation is relative.
	X = Roll, Y = Pitch, Z = Yaw. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Curves")
	class UCurveVector* SplineRotationCurve;

	/* World size of CloseText on mouse over. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Close Text")
	float CloseTextMouseOverSize;

	/* World size of CloseText on mouse over end. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Close Text")
	float CloseTextMouseOverEndSize;

	/* Map of position on the page to the text. Keys with value less than 0 do nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text")
	TMap<int32, FString> PageText;

	/* The initial (relative) location of the first line of text. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Transforms")
	FVector InitialTextLocation;

	/* The initial (relative) rotation of the text. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Transforms")
	FRotator InitialTextRotation;

	/* This is the InitialTextTransform.Location() - (NextTextAdjustLocation * Position). In other words, how much more
	"down" the next line is. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Transforms")
	FVector NextTextAdjustLocation;

	/* The maximal allowed text length (i.e. number of characters) when adding a new line of text. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|General Configuration")
	int32 MaxTextLength;

	/* The maximal allowed position (the number of lines) on the page. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|General Configuration")
	int32 MaxNumberOfLines;

	/* The text material. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Text Configuration")
	class UMaterialInterface* TextMaterial;

	/* The text material. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Text Configuration")
	class UFont* TextFont;

	/* The text color. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Text Configuration")
	FColor TextColor;

	/* The text X Scale. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Text Configuration")
	float TextXScale;

	/* The text Y Scale. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Text Configuration")
	float TextYScale;

	/* The text World Size. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Text Configuration")
	float TextWorldSize;

	/* The text Vertical Alignment. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Text Configuration")
	TEnumAsByte<enum EVerticalTextAligment> TextVerticalAligment;

	/* The text Horizontal Alignment. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Clipboard|Page Text|Text Configuration")
	TEnumAsByte<enum EHorizTextAligment> TextHorizontalAligment;

private:
	/* Timeline component used to traverse spline. */
	UPROPERTY()
	class UTimelineComponent* SplineTimeline;

	/* Flag for whether or not the clipboard is zoomed in. */
	bool bIsZoomedIn;

	/* Flag for whether the timeline was properly configured or not. */
	bool bIsSplineTimelineConfigured;

	/* The initial collision type for the selection bounds. */
	ECollisionEnabled::Type SelectionBoundsCollisionEnabledType;

/* --- FUNCTIONS --- */
public:
	/** Constructor.
	  */
	AInvestigationClipboard();

	/** Adds text to the given line. Bear in mind we start counting from 0 rather than 1 (so 0 is the first line).
	  * @param TextToAdd - The text to add.
	  * @param Position - The position to add the text at. If there is text already at this position, it will be over-written.
	  */
	UFUNCTION(BlueprintCallable, Category = "Investigation Clipboard")
	void AddTextToPage(FString TextToAdd, int32 Position);

protected:
	/** Begin play to configure spline timeline, configure collisions and initialise intial text.
	  */
	virtual void BeginPlay() override;

	/** Called when click on component happens.
	  * @param TouchedComponent - The component clicked.
	  */
	virtual void OnMouseClicked_Implementation(class UPrimitiveComponent* TouchedComponent) override;

	/** In the case of the investigation clipboard, we only want the base StaticMesh to render
	  * in custom depth
	  * @param RenderInCustomDepth - True to render in custom depth.
	  */
	virtual void SetComponentRenderCustomDepth(bool RenderInCustomDepth) override;

private:
	/** Call to configure the spline timeline.
	  */
	void ConfigureSplineTimeline();

	/** Callback for Spline Timeline on SplineTraversalCurve.
	  * @param Value - Value from timeline float curve.
	  */
	UFUNCTION()
	void SplineTimelineCallback(float Value);

	/** Callback for Spline Timeline on SplineRotationCurve.
	  * @param Value - Value from timeline vector curve.
	  */
	UFUNCTION()
	void SplineRotationCallback(FVector Value);

	/** On timeline finished function for Spline Timeline.
	  */
	UFUNCTION()
	void SplineTimelineOnEnd();

	/* --- MOUSE EVENTS --- */

	/** Function which sets CloseText->Size to CloseTextMouseOver size.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	UFUNCTION()
	void CloseCollisionMouseOver(class UPrimitiveComponent* TouchedComponent);

	/** Function which calls reverses the clipboard back to its initial position.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	UFUNCTION()
	void CloseCollisionClicked(class UPrimitiveComponent* TouchedComponent, FKey PressedKey);

	/** Function which sets CloseText->Size to CloseTextMouseOverEnd size.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	UFUNCTION()
	void CloseCollisionMouseOverEnd(class UPrimitiveComponent* TouchedComponent);
};
