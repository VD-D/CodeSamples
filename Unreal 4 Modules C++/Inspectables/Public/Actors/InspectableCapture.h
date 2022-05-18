// Copyright Robert Zygmunt Uszynski 2021-2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InspectableCapture.generated.h"

UCLASS()
class INSPECTABLES_API AInspectableCapture : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneCaptureComponent2D* FinalColorSceneCapture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneCaptureComponent2D* OpacitySceneCapture;

	/* Class of inspectable widget. Note that changing this after the first time the widget is created does nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspectable Capture", meta = (BlueprintBaseOnly))
	TSubclassOf<class UInspectableWidget> InspectableWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Inspectable Capture")
	int32 InspectableWidgetZOrder;

private:
	/* Mark AInspectableCapture as friend class. */
	friend class AInGameInspectable;

	class UInspectableWidget* CurrentInspectableWidget;

/* --- FUNCTIONS --- */
public:	
	/**
	 * Constructor. 
	 */
	AInspectableCapture();

	/**
	 * Empties show only list and destroys all referenced actors.
	 */
	void DestroyReferencedActors();

private:
	/**
	 * Intended to be called by AInGameInspectable to initialise the widget.
	 * @param Caller The InGameInspectable which called the capture begin.
	 */
	void CaptureBegin(class AInGameInspectable* Caller, const FText& PassedText);
};
