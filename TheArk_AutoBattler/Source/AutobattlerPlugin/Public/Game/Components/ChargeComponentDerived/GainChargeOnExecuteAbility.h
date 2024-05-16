// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Components/AutobattlerChargeComponent.h"
#include "Types/AutobattlerEnums.h"
#include "GainChargeOnExecuteAbility.generated.h"

/**
 * Implementation of charge component which has the character gain a charge whenever they execute an ability.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UGainChargeOnExecuteAbility : public UAutobattlerChargeComponent
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
	 * @param AbilityType Checked to see if it is an ability.
	*/
	UFUNCTION()
	void OnCharacterAbility(EAbilityType AbilityType);
};
