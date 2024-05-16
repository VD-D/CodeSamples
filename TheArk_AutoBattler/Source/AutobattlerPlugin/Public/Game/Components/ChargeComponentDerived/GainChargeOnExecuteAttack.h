// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Components/AutobattlerChargeComponent.h"
#include "Types/AutobattlerEnums.h"
#include "GainChargeOnExecuteAttack.generated.h"

/**
 * Implementation of charge component which has the character gain a charge whenever they execute an attack.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UGainChargeOnExecuteAttack : public UAutobattlerChargeComponent
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

private:
	/**
	 * Called whenever the character executes a skill.
	 * @param AbilityType Checked to see if it is an attack.
	*/
	UFUNCTION()
	void OnCharacterAttack(EAbilityType AbilityType);
};
