// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayActors/MouseSelectable.h"
#include "InvestigationPin.generated.h"

/* "Pins" stuck into Proloque's map that allow the player to upload investigation footage. */
UCLASS()
class SACRUSMORDUM_API AInvestigationPin : public AMouseSelectable
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- COMPONENTS --- */

	/* Mesh representing the pin. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Pin Components")
	class UStaticMeshComponent* PinMesh;

	/* Mesh which "outlines" the pin. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Pin Components")
	class UStaticMeshComponent* Outline;

	/* Widget component which displays information about the location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Pin Components")
	class UWidgetComponent* InfoDisplay;

	/* --- CONFIGURABLE --- */

	/* Text which gives a description of the location the pin is stuck into. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Pin")
	FText DescriptionText;

	/* Name of the level that should be loaded when clicking on the pin. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Pin")
	FName StreamLevelName;

private:
	bool bShowOutline;

/* --- FUNCTIONS --- */
public:
	/** Constructor.
	  */
	AInvestigationPin();

	/* --- BLUEPRINT EXPOSED --- */

	/** Whether or not to display the outline.
	  * @param ShowOutline - Show if true.
	  */
	UFUNCTION(BlueprintCallable, Category = "Investigation Pin")
	void SetShowOutline(bool ShowOutline);

protected:
	/** Called when mouse over happens.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	virtual void OnMouseOver_Implementation(class UPrimitiveComponent* TouchedComponent) override;

	/** Called when click on component happens.
	  * @param TouchedComponent - The component clicked.
	  */
	virtual void OnMouseClicked_Implementation(class UPrimitiveComponent* TouchedComponent) override;

	/** Called when mouse over ends.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	virtual void OnMouseOverEnd_Implementation(class UPrimitiveComponent* TouchedComponent) override;

private:
	/** Called when the streaming level is loaded from the investigation pin.
	  */
	UFUNCTION()
	void OnInvestigationLevelLoaded();
};
