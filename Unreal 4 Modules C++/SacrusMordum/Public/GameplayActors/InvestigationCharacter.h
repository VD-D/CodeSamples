// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InvestigationCharacter.generated.h"

enum class EInvestigationDiscoveryInterfaceType : uint8
{
	Found,
	Lost
};

UCLASS()
class SACRUSMORDUM_API AInvestigationCharacter : public ACharacter
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- COMPONENTS --- */

	/* Player camera. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Character Components")
	class UCameraComponent* Camera;

	/* Sphere used to "overlap" investigation objects. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Character Components")
	class UCapsuleComponent* InvestigationTrace;

	/* --- CONFIGURABLE --- */

	/* Pointer to the world widget manager. If not set, will attempt to find all actors of
	ASacrusMordumWidgetManager and use the first one it finds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Character|Configurable|Managers")
	class ASacrusMordumWidgetManager* WorldWidgetManager;

private:
	/* --- INTERNAL USE --- */

	/* Pointer to current investigation object. */
	class AInvestigationObject* CurrentInvestigationObject;

	/* Whether or not the focusing "beam" is currently switching widgets. */
	bool bIsSwitchingFocus;


/* --- FUNCTIONS --- */
public:
	/* --- CONSTRUCTION --- */

	/** Constructor.
	  */
	AInvestigationCharacter();

protected:
	/** Begin play for configuring FocusTimeline + delegates + FocusWidget.
	  */
	virtual void BeginPlay() override;

private:
	/* --- DELEGATE BOUND --- */

	/** Function bound to InvestigationTrace beginning overlaps.
	  * @param OverlappedComponent - Unused.
	  * @param OtherActor - Tested to see if it is an investigation object other than the current one.
	  * @param OtherComp - Unused.
	  * @param OtherBodyIndex - Unused.
	  * @param bFromSweep - Unused.
	  * @param SweepResult - Unused.
	  */
	UFUNCTION()
	void InvestigationTraceBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	/** Function bound to InvestigationTrace ending overlaps.
	  * @param OverlappedComponent - Unused.
	  * @param OtherActor - Tested to see if it is an investigation object other than the current one.
	  * @param OtherComp - Unused.
	  * @param OtherBodyIndex - Unused.
	  */
	UFUNCTION()
	void InvestigationTraceEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* --- FOCUS --- */

	/** Begins playing the focus timeline (from the start) when InvestigationTrace overlaps an investigation object.
	  */
	void OnFocusBegin();

	/** Occurs if focus is interrupted, e.g. when InvestigationTrace stops overlapping an investigation object or switches to a different one.
	  */
	void OnFocusInterrupted();

	/* --- INVESTIGATION DISCOVER INTERFACE --- */

	/** Finds all actors and widgets which implement the investigation discovery interface.
	  * @param InvestigationDiscoveryType - The type of function to execute.
	  */
	void ExecuteInvestigationDiscoveryInterface(EInvestigationDiscoveryInterfaceType InvestigationDiscoveryType);
};
