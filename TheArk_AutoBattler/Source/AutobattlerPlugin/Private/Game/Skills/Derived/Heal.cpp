// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Skills/Derived/Heal.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"

void UHeal::ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation)
{
    if (SkillTargetingMode != ESkillTargetingMode::Actor || !IsValid(Target)) return;
    Target->SetCurrentHealth(Target->GetMaxHealth() + MinMaxHeal.GetRandomValueInRange());
}
