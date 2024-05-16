// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Skills/Derived/Ressurect.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"

void URessurect::ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation)
{
    if (IsValid(Target))
    {
        Target->SetCanRessurect();
    }
}
