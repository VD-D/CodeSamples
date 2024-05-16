// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Components/ChargeComponentDerived/GainChargeOnLoseLife.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"

void UGainChargeOnLoseLife::Initialise_Implementation(AAutobattlerCharacter* OwnerCharacter, AAutobattlerAIController* OwnerCharacterController)
{
    if (ensureAlwaysMsgf(IsValid(OwnerCharacter), TEXT("%s : [Initialise_Implementation] Does not have valid owner character!")))
    {
        OwnerCharacter->OnHealthLost.AddDynamic(this, &UGainChargeOnLoseLife::CharacterGainCharge);
    }
}
