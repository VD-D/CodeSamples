// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AutobattlerMouseInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAutobattlerMouseInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface which handles begin/end and click mouse from the autobattler pawn.
 */
class AUTOBATTLERPLUGIN_API IAutobattlerMouseInterface
{
	GENERATED_BODY()

public:
	/**
	 * Function called when the mouse begins an overlap.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Mouse Interface")
	void OnMouseBeginOverlap();

	/**
	 * Function called when the mouse ends its overlap.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Mouse Interface")
	void OnMouseEndOverlap();

	/**
	 * Function called when the mouse clicks on this actor using the primary key.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Mouse Interface")
	void OnMousePrimary();

	/**
	 * Function called when the mouse clicks not on the actor. 
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Mouse Interface")
	void OnDeselect();
};
