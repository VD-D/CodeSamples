// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Components/AutobattlerChargeComponent.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"

void UAutobattlerChargeComponent::Initialise_Implementation(AAutobattlerCharacter* OwnerCharacter, AAutobattlerAIController* OwnerCharacterController)
{
	// Dummy implementation
}

void UAutobattlerChargeComponent::CharacterGainCharge()
{
	AAutobattlerCharacter* OwningCharacter = Cast<AAutobattlerCharacter>(GetOwner());
	if (ensureAlwaysMsgf(IsValid(OwningCharacter), TEXT("%s : [CharacterGainCharge] Owner is not an autobattler character!"), *GetName()))
	{
		OwningCharacter->GainCharge();
	}
}
