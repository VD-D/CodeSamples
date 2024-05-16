// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Skills/Derived/ModifyStat.h"

/* Autobattler includes. */
#include "Game/Components/CharacterPanelComponent.h"
#include "Game/Units/AutobattlerCharacter.h"

void UModifyStat::ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation)
{
    if (IsValid(Target))
    {
        Target->ModifyStatistic(StatToModify, ModifierScale, IsPermanent, FMath::Max(ExpiryTime, 0.1f));
        if (UseModifier) Target->GetCharacterPanelComponent()->AddModifier(ModifierIcon, ModifierDescription, IsPermanent ? -1.0f : FMath::Max(ExpiryTime, 0.1f));
    }
}
