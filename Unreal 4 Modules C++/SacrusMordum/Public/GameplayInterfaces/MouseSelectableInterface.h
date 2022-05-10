// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../SharedStructures/MouseSelectableMetaInfo.h"
#include "MouseSelectableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMouseSelectableInterface : public UInterface
{
	GENERATED_BODY()
};

/* Interface for handling MouseSelectable events. */
class SACRUSMORDUM_API IMouseSelectableInterface
{
	GENERATED_BODY()

public:
	/** Called when mouse over starts on a MouseSelectableObject.
	  * @param TouchedComponent - Component touched by the mouse.
	  * @param MouseSelectableInfo - Information about the mouse selectable.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mouse Selectable Interface")
	void OnMouseSelectableMouseOverStart(class UPrimitiveComponent* TouchedComponent, FMouseSelectableMetaInfo MouseSelectableInfo);

	/** Called when mouse is clicked on a MouseSelectableObject.
	  * @param TouchedComponent - Component touched by the mouse.
	  * @param PressedKey - The button (or mouse key) pressed to trigger the click.
	  * @param MouseSelectableInfo - Information about the mouse selectable.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mouse Selectable Interface")
	void OnMouseSelectableMouseClicked(class UPrimitiveComponent* TouchedComponent, FKey PressedKey, FMouseSelectableMetaInfo MouseSelectableInfo);

	/** Called when mouse over ends on a MouseSelectableObject.
	  * @param TouchedComponent - Component touched by the mouse.
	  * @param MouseSelectableInfo - Information about the mouse selectable.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mouse Selectable Interface")
	void OnMouseSelectableMouseOverEnd(class UPrimitiveComponent* TouchedComponent, FMouseSelectableMetaInfo MouseSelectableInfo);
};
