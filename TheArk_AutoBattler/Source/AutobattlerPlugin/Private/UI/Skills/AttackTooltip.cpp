// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/Skills/AttackTooltip.h"

/* Autobattler includes. */
#include "DataAssets/AutobattlerAttack.h"

/* Engine includes. */
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

void UAttackTooltip::BuildTooltipFromAsset(const UAutobattlerAttack* AttackAsset)
{
    if (!IsValid(AttackAsset)) return;

    if (ensureAlwaysMsgf(
        IsValid(AbilityIcon) &&
        IsValid(AttackName) &&
        IsValid(AttackDescription) &&
        IsValid(AttackRange) &&
        IsValid(AttackActionSpeed) &&
        IsValid(AttackTargetDescription)
    ,
    TEXT("%s : [BuildTooltipFromAsset] There are components which are invalid!"), *GetName()))
    {
        AbilityIcon->SetBrushFromTexture(AttackAsset->SkillIcon);
        AttackName->SetText(AttackAsset->SkillName);
        AttackDescription->SetText(AttackAsset->SkillDescription);
        AttackRange->SetText(FText::AsNumber(AttackAsset->Range));
        AttackActionSpeed->SetText(FText::AsNumber(AttackAsset->ActionSpeed));
        AttackTargetDescription->SetText(AttackAsset->TargetDescription);
        OnAbilityTooltipBuilt(AttackAsset);
    }
}

void UAttackTooltip::OnAbilityTooltipBuilt_Implementation(const UAutobattlerAttack* AttackAsset)
{
    // Dummy implementation
}
