// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/Deployment/CharacterPanel.h"

/* Autobattler includes. */
#include "DataAssets/AutobattlerAbility.h"
#include "DataAssets/AutobattlerAttack.h"
#include "UI/Deployment/CharacterPanelComponents/AutobattlerDiscreteStatWidget.h"
#include "UI/Deployment/CharacterPanelComponents/AutobattlerStatWidget.h"
#include "UI/Deployment/CharacterPanelComponents/ModifierIcon.h"
#include "UI/Skills/SkillIcon.h"

/* Engine includes. */
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

void UCharacterPanel::BuildCharacterPanel(UTexture2D* CharacterIcon, const FText& CharacterName, const FCharacterBaseStats& CharacterBaseStats, const UAutobattlerAttack* AttackAsset, const UAutobattlerAbility* AbilityAsset)
{
    if (ensureAlwaysMsgf(IsValid(AttackAsset) && IsValid(AbilityAsset), TEXT("%s : [BuildCharacterPanel] Passed ability or attack asset (or both) were invalid!"), *GetName()))
    {
        if (ensureAlwaysMsgf(
            IsValid(CharacterPortrait) &&
            IsValid(CharacterNameText) &&
            IsValid(ModifiersBox) &&
            IsValid(AbilityIcon) &&
            IsValid(AttackIcon), 
            TEXT("%s : [BuildCharacterPanel] Some widget components are not valid!"), *GetName()
        ))
        {
            CharacterPortrait->SetBrushFromTexture(CharacterIcon);
            CharacterNameText->SetText(CharacterName);
            AbilityIcon->SetSkillDataAsset(AbilityAsset);
            AttackIcon->SetSkillDataAsset(AttackAsset);
            UpdateStat(EAutobattlerStatType::AttackSpeed, AttackAsset->ActionSpeed, true);
            UpdateStat(EAutobattlerStatType::SkillSpeed, AbilityAsset->ActionSpeed, true);
            UpdateStat(EAutobattlerStatType::MaxHealth, CharacterBaseStats.BaseHealth, true);
            UpdateStat(EAutobattlerStatType::MovementSpeed, CharacterBaseStats.BaseMovementSpeed, true);
            UpdateStat(EAutobattlerStatType::CriticalChance, CharacterBaseStats.BaseCriticalChance, true);
            UpdateStat(EAutobattlerStatType::CriticalMultiplier, CharacterBaseStats.BaseCriticalMultiplier, true);
            UpdateStat(EAutobattlerDiscreteType::DamageType, CharacterBaseStats.BaseDamageType, true);
            UpdateStat(EAutobattlerDiscreteType::ResistanceType, CharacterBaseStats.BaseResistanceType, true);
        }
    }
}

void UCharacterPanel::AddModifier(UTexture2D* IconImage, const FText& ModifierDescription, float Duration)
{
    if (ensureAlwaysMsgf(IsValid(ModifiersBox) && ModifierIconClass.Get() != nullptr, TEXT("%s : [AddModifier] Modifiers Box WrapBox widget is invalid or ModifierIconClass is null!"), *GetName()))
    {
        UModifierIcon* ModifierIcon = CreateWidget<UModifierIcon>(GetWorld(), ModifierIconClass);
        ModifierIcon->BuildModifierIcon(IconImage, ModifierDescription, Duration);
        ModifiersBox->AddChildToWrapBox(ModifierIcon);
    }
}

void UCharacterPanel::UpdateStat(EAutobattlerStatType StatTypeToModify, float NewValue, bool IsBase)
{
    if (ensureAlwaysMsgf(IsValid(WidgetTree), TEXT("%s : [UpdateStat] Widget Tree is invalid!"), *GetName()))
    {
        TArray<UWidget*> Children;
        WidgetTree->GetAllWidgets(Children);

        for (auto Child : Children)
        {
            if (UAutobattlerStatWidget* StatWidget = Cast<UAutobattlerStatWidget>(Child))
            {
                if (StatWidget->ModifiedStatType == StatTypeToModify)
                {
                    StatWidget->UpdateValue(IsBase, NewValue);
                }
            }
        }
    }
}

void UCharacterPanel::UpdateStat(EAutobattlerDiscreteType StatTypeToModify, const FText& NewValue, bool IsBase)
{
    if (ensureAlwaysMsgf(IsValid(WidgetTree), TEXT("%s : [UpdateStat] Widget Tree is invalid!"), *GetName()))
    {
        TArray<UWidget*> Children;
        WidgetTree->GetAllWidgets(Children);

        for (auto Child : Children)
        {
            if (UAutobattlerDiscreteStatWidget* StatWidget = Cast<UAutobattlerDiscreteStatWidget>(Child))
            {
                if (StatWidget->ModifiedStatType == StatTypeToModify)
                {
                    if (IsBase) StatWidget->SetBaseText(NewValue);
                    StatWidget->UpdateText(NewValue);
                }
            }
        }
    }
}
