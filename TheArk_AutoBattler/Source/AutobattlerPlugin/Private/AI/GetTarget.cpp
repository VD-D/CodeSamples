// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/GetTarget.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"

ESkillTargetingMode UGetTarget::GetTarget_Implementation(AAutobattlerCharacter* OwningCharacter, const TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter*& TargetCharacter, FVector& TargetLocation)
{
    TArray<AAutobattlerCharacter*> CharactersToChooseFrom = FilteredCharacters;
    if (CharactersToChooseFrom.Contains(OwningCharacter)) CharactersToChooseFrom.Remove(OwningCharacter);

    TargetCharacter = nullptr;
    if (CharactersToChooseFrom.Num() > 0) TargetCharacter = CharactersToChooseFrom[FMath::RandRange(0, CharactersToChooseFrom.Num() - 1)];

    return TargetCharacter != nullptr ? ESkillTargetingMode::Actor : ESkillTargetingMode::None;
}
