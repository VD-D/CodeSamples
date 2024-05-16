// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/GetTargetDerived/GetNearestTarget.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"
#include "Utility/AutobattlerFunctionLibrary.h"

ESkillTargetingMode UGetNearestTarget::GetTarget_Implementation(AAutobattlerCharacter* OwningCharacter, const TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter*& TargetCharacter, FVector& TargetLocation)
{
    if (!IsValid(OwningCharacter)) return ESkillTargetingMode::None;

    TArray<AAutobattlerCharacter*> CharactersToChooseFrom = FilteredCharacters;
    if (CharactersToChooseFrom.Contains(OwningCharacter)) CharactersToChooseFrom.Remove(OwningCharacter);

    if (CharactersToChooseFrom.Num() < 1) return ESkillTargetingMode::None;

    float LargestRange = MAX_FLT;
    for (auto Character : CharactersToChooseFrom)
    {
        if (!IsValid(Character)) continue;

        float Distance = UAutobattlerFunctionLibrary::CartesianDistance(OwningCharacter->GetActorLocation(), Character->GetActorLocation());
        if (Distance < LargestRange)
        {
            TargetCharacter = Character;
            LargestRange = Distance;
        }
    }
    
    return ESkillTargetingMode::Actor;
}
