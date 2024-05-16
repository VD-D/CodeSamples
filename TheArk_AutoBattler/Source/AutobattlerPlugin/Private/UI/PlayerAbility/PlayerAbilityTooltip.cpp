// Copyright Juggler Games 2022 - 2024
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/PlayerAbility/PlayerAbilityTooltip.h"

/* UE4 includes. */
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

void UPlayerAbilityTooltip::BuildTooltip(FAutobattlerPlayerAbilityCosmetic CosmeticData)
{
    if (IsValid(AbilityName)) AbilityName->SetText(CosmeticData.PlayerSkillName);
    if (IsValid(AbilityDescription)) AbilityDescription->SetText(CosmeticData.PlayerSkillDescription);
}
