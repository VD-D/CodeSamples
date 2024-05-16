// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/GetTargetDerived/GetSelfAsTarget.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"

ESkillTargetingMode UGetSelfAsTarget::GetTarget_Implementation(AAutobattlerCharacter* OwningCharacter, const TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter*& TargetCharacter, FVector& TargetLocation)
{
    if (!IsValid(OwningCharacter))
    {
        TargetCharacter = nullptr;
        return ESkillTargetingMode::None;
    }

    TargetCharacter = OwningCharacter;
    return ESkillTargetingMode::Actor;
}
