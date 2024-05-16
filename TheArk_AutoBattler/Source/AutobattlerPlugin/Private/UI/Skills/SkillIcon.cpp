// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/Skills/SkillIcon.h"

/* Autobattler includes. */
#include "DataAssets/AutobattlerAbility.h"
#include "DataAssets/AutobattlerAttack.h"
#include "UI/Skills/AbilityTooltip.h"
#include "UI/Skills/AttackTooltip.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USkillIcon::SetSkillDataAsset(const UAutobattlerSkill* SkillAsset)
{
    if (!IsValid(SkillAsset)) return;
    CurrentSkill = SkillAsset;

    if (ensureAlwaysMsgf(IsValid(SkillIconImage), TEXT("%s : [SetSkillIconAndName] SkillIconImage Image widget is invalid!"), *GetName()))
    {
        SkillIconImage->SetBrushFromTexture(CurrentSkill->SkillIcon);
    }

    if (IsValid(SkillNameText)) // May be invalid since it is an optional widget binding
    {
        SkillNameText->SetText(CurrentSkill->SkillName);
    }

    ToolTipWidget = GetSkillWidget();
}

UWidget* USkillIcon::GetSkillWidget()
{
    if (const UAutobattlerAbility* Ability = Cast<UAutobattlerAbility>(CurrentSkill))
    {
        if (ensureAlwaysMsgf(AbilityTooltipClass.Get() != nullptr, TEXT("%s : [GetSkillWidget] Ability tooltip does not have valid class!"), *GetName()))
        {
            UAbilityTooltip* AbilityTooltip = CreateWidget<UAbilityTooltip>(GetWorld(), AbilityTooltipClass);
            AbilityTooltip->BuildTooltipFromAsset(Ability);
            return AbilityTooltip;
        }
    }

    if (const UAutobattlerAttack* Attack = Cast<UAutobattlerAttack>(CurrentSkill))
    {
        if (ensureAlwaysMsgf(AttackTooltipClass.Get() != nullptr, TEXT("%s : [GetSkillWidget] Attack tooltip does not have valid class!"), *GetName()))
        {
            UAttackTooltip* AttackTooltip = CreateWidget<UAttackTooltip>(GetWorld(), AttackTooltipClass);
            AttackTooltip->BuildTooltipFromAsset(Attack);
            return AttackTooltip;
        }
    }

    UAutobattlerFunctionLibrary::PrintErrorToLog(FString("Tooltip type did not have data asset set."));
    return nullptr;
}
