// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Components/ChargeComponentDerived/GainChargeOnExecuteAttack.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"

void UGainChargeOnExecuteAttack::Initialise_Implementation(AAutobattlerCharacter* OwnerCharacter, AAutobattlerAIController* OwnerCharacterController)
{
    if (ensureAlwaysMsgf(IsValid(OwnerCharacterController), TEXT("%s : [Initialise_Implementation] Does not have valid AI Controller!")))
    {
        OwnerCharacterController->OnSkillExecuted.AddDynamic(this, &UGainChargeOnExecuteAttack::OnCharacterAttack);
    }
}

void UGainChargeOnExecuteAttack::OnCharacterAttack(EAbilityType AbilityType)
{
    if (AbilityType == EAbilityType::Attack)
    {
        CharacterGainCharge();
    }
}
