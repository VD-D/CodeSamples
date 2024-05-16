// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/Skills/AbilityTooltip.h"

/* Autobattler includes. */
#include "DataAssets/AutobattlerAbility.h"

/* Engine includes. */
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

void UAbilityTooltip::BuildTooltipFromAsset(const UAutobattlerAbility* AbilityAsset)
{
    if (!IsValid(AbilityAsset)) return;

    if (ensureAlwaysMsgf(
        IsValid(AbilityIcon) &&
        IsValid(SkillName) &&
        IsValid(SkillDescription) &&
        IsValid(SkillRange) &&
        IsValid(SkillActionSpeed) &&
        IsValid(SkillTargetDescription) &&
        IsValid(SkillCounter) &&
        IsValid(GainChargesCondition)
    ,
    TEXT("%s : [BuildTooltipFromAsset] There are components which are invalid!"), *GetName()))
    {
        AbilityIcon->SetBrushFromTexture(AbilityAsset->SkillIcon);
        SkillName->SetText(AbilityAsset->SkillName);
        SkillDescription->SetText(AbilityAsset->SkillDescription);
        SkillRange->SetText(FText::AsNumber(AbilityAsset->Range));
        SkillActionSpeed->SetText(FText::AsNumber(AbilityAsset->ActionSpeed));
        SkillTargetDescription->SetText(AbilityAsset->TargetDescription);
        if (AbilityAsset->AbilityTriggerType == EAbilityTriggerType::Cooldown)
        {
            SkillCounter->SetText(FText::AsNumber(AbilityAsset->Cooldown));
            GainChargesCondition->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            SkillCounter->SetText(FText::AsNumber(AbilityAsset->ChargesNeededForTrigger));
            GainChargesCondition->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            GainChargesCondition->SetText(AbilityAsset->ChargeGainDescription);
        }

        OnAbilityTooltipBuilt(AbilityAsset);
    }
}

void UAbilityTooltip::OnAbilityTooltipBuilt_Implementation(const UAutobattlerAbility* AbilityAsset)
{
    // Dummy implementation
}
