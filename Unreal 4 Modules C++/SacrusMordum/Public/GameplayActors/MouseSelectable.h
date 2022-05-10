// Copyright Robert Zygmunt Uszynski 2020-2021
#pragma once

#include "CoreMinimal.h"
#include "../SharedStructures/MouseSelectableMetaInfo.h"
#include "GameFramework/Actor.h"
#include "MouseSelectable.generated.h"

UENUM(BlueprintType)
enum class EInterfaceFunctionType : uint8
{
	MouseOverStart UMETA(DisplayName = "Mouse Over Start"),
	Clicked UMETA(DisplayName = "Clicked"),
	MouseOverEnd UMETA(DisplayName = "Mouse Over End")
};

UCLASS()
class SACRUSMORDUM_API AMouseSelectable : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* Scene root. Useful if wanting to move box component without moving the actor itself. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mouse Selectable Components")
	class USceneComponent* SceneRoot;

	/* Box bounds used to define a selection region in 3D space. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mouse Selectable Components")
	class UBoxComponent* SelectionBounds;

	/* Meta info about this widget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Selectable Configurable")
	FMouseSelectableMetaInfo MetaInfo;

/* --- FUNCTIONS --- */
public:	
	/** Constructor.
	  */
	AMouseSelectable();

protected:
	/** Begin play to bind delegates.
	  */
	virtual void BeginPlay() override;

	/** Called when mouse over happens.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mouse Selectable")
	void OnMouseOver(class UPrimitiveComponent* TouchedComponent);
	virtual void OnMouseOver_Implementation(class UPrimitiveComponent* TouchedComponent);

	/** Called when click on component happens.
	  * @param TouchedComponent - The component clicked.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mouse Selectable")
	void OnMouseClicked(class UPrimitiveComponent* TouchedComponent);
	virtual void OnMouseClicked_Implementation(class UPrimitiveComponent* TouchedComponent);

	/** Called when mouse over ends.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mouse Selectable")
	void OnMouseOverEnd(class UPrimitiveComponent* TouchedComponent);
	virtual void OnMouseOverEnd_Implementation(class UPrimitiveComponent* TouchedComponent);

private:
	/* --- MOUSE EVENTS --- */

	/** Function which calls MouseSelectableInterface->OnMouseSelectableMouseOverStart interface and provides 
	  * MouseSelectableMetaInfo to all listening actors and widgets.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	UFUNCTION()
	void MouseSelectableMouseOver(class UPrimitiveComponent* TouchedComponent);

	/** Function which calls  MouseSelectableInterface->OnMouseSelectableMouseClicked interface and provides
	  * MouseSelectableMetaInfo to all listening actors and widgets.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	UFUNCTION()
	void MouseSelectableClicked(class UPrimitiveComponent* TouchedComponent, FKey PressedKey);

	/** Function which calls  MouseSelectableInterface->OnMouseSelectableMouseOverEnd interface and provides
	  * MouseSelectableMetaInfo to all listening actors and widgets.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	UFUNCTION()
	void MouseSelectableMouseOverEnd(class UPrimitiveComponent* TouchedComponent);

	/* --- UTILITY --- */

	/** Executes mouse selectable interface on ALL listening objects.
	  * @param FunctionCallType - The type of function to execute.
	  * @param TouchedComponent - The component touched by the mouse.
	  */
	UFUNCTION(BlueprintCallable, Category = "Mouse Selectable", meta = (DisplayName = "Call Mouse Over Interface"))
	void CallMouseSelectableInterfaceOnListeners(EInterfaceFunctionType FunctionCallType, class UPrimitiveComponent* TouchedComponent = nullptr);
};
