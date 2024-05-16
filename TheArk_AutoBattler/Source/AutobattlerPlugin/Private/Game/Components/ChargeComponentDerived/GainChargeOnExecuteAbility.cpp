// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Components/ChargeComponentDerived/GainChargeOnExecuteAbility.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"

void UGainChargeOnExecuteAbility::Initialise_Implementation(AAutobattlerCharacter* OwnerCharacter, AAutobattlerAIController* OwnerCharacterController)
{
    if (ensureAlwaysMsgf(IsValid(OwnerCharacterController), TEXT("%s : [Initialise_Implementation] Does not have valid AI Controller!")))
    {
        OwnerCharacterController->OnSkillExecuted.AddDynamic(this, &UGainChargeOnExecuteAbility::OnCharacterAbility);
    }
}

void UGainChargeOnExecuteAbility::OnCharacterAbility(EAbilityType AbilityType)
{
    if (AbilityType == EAbilityType::Skill)
    {
        CharacterGainCharge();
    }
}
