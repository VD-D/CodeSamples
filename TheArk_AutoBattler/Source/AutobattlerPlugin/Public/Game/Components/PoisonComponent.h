// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoisonComponent.generated.h"

/**
 * Component responsible for dealing poison damage to the character it is attached to.
 */
UCLASS(ClassGroup=(Autobattler))
class AUTOBATTLERPLUGIN_API UPoisonComponent : public UActorComponent
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Timer handle used to manage poison counter. */
	FTimerHandle PoisonTimerHandle;

	/* How strong the poison currently is. */
	float CurrentPoisonStrength;

/////////////////////////////////////////////////////////////////////////////////
//// API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Applies the poison of a given strength.
	 * @param PosionStrength How much damage the poison does per tick (reduced per tick)
	 * @see AutobattlerConfiguration.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ApplyPoison(float PoisonStrength);

private:
	UFUNCTION()
	void TickPoison();	
};
