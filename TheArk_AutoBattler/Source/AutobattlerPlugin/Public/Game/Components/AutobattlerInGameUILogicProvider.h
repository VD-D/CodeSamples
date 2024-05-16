// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutobattlerInGameUILogicProvider.generated.h"

/**
 * Component intended to be overriden in blueprints.
 * Provides "logic" for whether or not in-game UI elements (i.e. widget components) are shown and when they are hidden.
 */
UCLASS(Blueprintable, ClassGroup=(Autobattler), meta=(BlueprintSpawnableComponent))
class AUTOBATTLERPLUGIN_API UAutobattlerInGameUILogicProvider : public UActorComponent
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Default constructor.
	 */
	UAutobattlerInGameUILogicProvider();

/////////////////////////////////////////////////////////////////////////////////
//// LOGIC INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Logic for whether or not in game UI elements should be shown.
	 * @return Whether or not in game UI elements should be shown.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Autobattler")
	bool ShouldShowInGameUIElements();
	virtual bool ShouldShowInGameUIElements_Implementation();
};
