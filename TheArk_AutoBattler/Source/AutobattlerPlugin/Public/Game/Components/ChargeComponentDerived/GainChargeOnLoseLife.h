// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Components/AutobattlerChargeComponent.h"
#include "GainChargeOnLoseLife.generated.h"

/**
 * Implementation of charge component which has the character gain a charge whenever they lose life.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UGainChargeOnLoseLife : public UAutobattlerChargeComponent
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// CHARGE API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Binds gaining charge to execution of attack.
	 * @param OwnerCharacter Character who owns this component.
	 * @param OwnerCharacterController Controller which controls owner character.
	 */
	virtual void Initialise_Implementation(AAutobattlerCharacter* OwnerCharacter, AAutobattlerAIController* OwnerCharacterController) override;
};
