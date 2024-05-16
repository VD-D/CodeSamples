// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutobattlerChargeComponent.generated.h"

class AAutobattlerAIController;
class AAutobattlerCharacter;

/**
 * Component responsible for setting up charge generation for its parent character.
 * Has one overridable function Initialise(), which is called when this component is created
 * and a utility function CharacterGainCharge, which makes the character gain a charge (when an event occurs).
 */
UCLASS(ClassGroup=(Autobattler))
class AUTOBATTLERPLUGIN_API UAutobattlerChargeComponent : public UActorComponent
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// CHARGE API
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * This function should bind some sort of event to executing CharacterGainCharge.
	 * @param OwnerCharacter Character who owns this component.
	 * @param OwnerCharacterController Controller which controls owner character.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Charge Component")
	void Initialise(AAutobattlerCharacter* OwnerCharacter, AAutobattlerAIController* OwnerCharacterController);
	virtual void Initialise_Implementation(AAutobattlerCharacter* OwnerCharacter, AAutobattlerAIController* OwnerCharacterController);

	/**
	 * Has the character gain a charge. If max charges gained, character will attempt to execute skill.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Charge Component")
	void CharacterGainCharge();
};
