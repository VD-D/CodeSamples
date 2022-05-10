// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SharedStructures/InvestigationInfoProperties.h"
#include "SacrusMordumWidgetManager.generated.h"

enum class EFocusInterfaceType : uint8
{
	FocusBegun,
	FocusInterrupted,
	FocusCompleted
};

/* Widget manager class, for all in-game UI related to Sacrus Mordum game. */
UCLASS()
class SACRUSMORDUM_API ASacrusMordumWidgetManager : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- COMPONENTS --- */

	/* Billboard component to make the widget manager easier to find in the map editor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Character Components")
	class UBillboardComponent* Billboard;

	/* --- FOCUS WIDGET --- */

	/* Curve to be used to fill out a progress bar. Values are clamped between 0 to 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sacrus Mordum Widget Manager|Configurable|Focus Widget")
	class UCurveFloat* FocusTimelineCurve;

	/* Class of the focus widget to create when focusing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sacrus Mordum Widget Manager|Configurable|Focus Widget")
	TSubclassOf<class UFocusWidget> FocusWidgetClass;

	/* Z-Order to set for the focus widget when constructing it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sacrus Mordum Widget Manager|Configurable|Focus Widget")
	int32 FocusWidgetZOrder;

	/* Whether or not the focus widget should be built on begin play, or done manually at a later stage by calling
	BuildFocusWidget(). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sacrus Mordum Widget Manager|Configurable|Focus Widget")
	bool bBuildFocusWidgetOnBeginPlay;

	/* --- INVESTIGATION INFO WIDGET --- */

	/* Class of the widget to create when presenting investigation info. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sacrus Mordum Widget Manager|Configurable|Investigation Info Widget")
	TSubclassOf<class UInvestigationInfoWidget> InvestigationInfoWidgetClass;

	/* Z-Order to set for the focus widget when constructing it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sacrus Mordum Widget Manager|Configurable|Investigation Info Widget")
	int32 InvestigationInfoWidgetZOrder;

	/* Whether or not the focus widget should be built on begin play, or done manually at a later stage by calling
	BuildFocusWidget(). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sacrus Mordum Widget Manager|Configurable|Investigation Info Widget")
	bool bBuildInvestigationInfoWidgetOnBeginPlay;

private:
	/* --- COMPONENTS --- */

	/* Timeline component which dictates the curve until an investigation object has been fully focused on. */
	UPROPERTY()
	class UTimelineComponent* FocusTimeline;

	/* --- INTERNAL USE --- */

	/* Pointer to currently constructed FocusWidget. */
	class UFocusWidget* CurrentFocusWidget;

	/* Pointer to currently constructed InvestigationInfoWidget. */
	class UInvestigationInfoWidget* CurrentInvestigationInfoWidget;

/* --- FUNCTIONS ---*/
public:	
	/* --- CONSTRUCTION --- */

	/** Constructor.
	  */
	ASacrusMordumWidgetManager();

	/* --- FOCUS WIDGET --- */

	/** Creates a focus widget from the supplied class. Will not build a second Focus Widget if one has been already built.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum Widget Manager", meta = (DisplayName = "Build Focus Widget"))
	void BuildFocusWidget();

	/** Occurs if focus is interrupted, e.g. when InvestigationTrace stops overlapping an investigation object or switches to a different one.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum Widget Manager", meta = (DisplayName = "Focus Interrupt"))
	void FocusInterrupt();

	/** Begins playing the focus timeline (from the start) when InvestigationTrace overlaps an investigation object.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum Widget Manager", meta = (DisplayName = "Focus Begin"))
	void FocusBegin();

	/* --- INVESTIGATION INFO WIDGET --- */

	/** Creates an investigation info widget from the supplied class. Will not build a second Investigation Info Widget if one has
	  * already been built.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum Widget Manager", meta = (DisplayName = "Build Investigation Info Widget"))
	void BuildInvestigationInfoWidget();

	/** Indicates to the constructed investigation info widget that it should play its entry/exit animation.
	  * NOTE: This DOES NOT actually play the animation, only calls the function on the built widget.
	  * @param Direction - Forward if true, backward if false.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum Widget Manager", meta = (DisplayName = "Play Transition Animation"))
	void InvestigationInfoWidgetPlayAnim(bool Direction);

	/** Updates the information on the built investigation info widget.
	  * @param NewInfo - The Investigation Info properties to pass.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum Widget Manager", meta = (DisplayName = "Update Investigation Info"))
	void UpdateInvestigationInfoWidget(FInvestigationObjectProperties NewInfo);

	/** Updates the scroll on the investigation info widget.
	  * @param AxisValue - A value which determines a scroll widget's offset. 0 = do not change, positive = scroll up, negatve = scroll down.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum Widget Manager", meta = (DisplayName = "Update Investigation Info Scrollbar"))
	void UpdateInvestigationInfoScroll(float AxisValue);

protected:
	/* --- CONSTRUCTION --- */

	/** Initialises widgets flagged to be built on begin play.
	  */
	virtual void BeginPlay() override;

private:
	/* --- FOCUS TIMELINE --- */

	/** Callback from FocusTimeline.
	  * @param Value - Current curve value.
	  */
	UFUNCTION()
	void WhileFocusing(float Value);

	/** Occurs if focus is not interrupted, i.e. FocusTimeline reaches the end.
	  */
	UFUNCTION()
	void OnFocusCompleted();

	/** Finds all actors and widgets with the IFocusInterface, and executes a function based on FocusInterfaceType on them.
	  * @param FocusInterfaceType - The function to execute.
	  */
	void ExecuteFocusInterface(EFocusInterfaceType FocusInterfaceType);
};
