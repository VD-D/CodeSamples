// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/WinCondition/WinConditionBase.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"

EWhoWins UWinConditionBase::CheckWinCondition_Implementation(const TArray<AAutobattlerCharacter*>& ParticipatingCharacters)
{
    bool DidEnemyWin = true;
    for (auto ParticipatingCharacter : ParticipatingCharacters)
    {
        if (!IsValid(ParticipatingCharacter)) continue;
        if (!ParticipatingCharacter->GetIsDead() && ParticipatingCharacter->GetOwnerIdentity() != EEntity::AI)
        {
            DidEnemyWin = false;
            break;
        }
    }

    if (DidEnemyWin) return EWhoWins::Enemy;

    bool DidPlayersWin = true;
    for (auto ParticipatingCharacter : ParticipatingCharacters)
    {
        if (!IsValid(ParticipatingCharacter)) continue;
        if (!ParticipatingCharacter->GetIsDead() && ParticipatingCharacter->GetOwnerIdentity() == EEntity::AI)
        {
            DidPlayersWin = false;
            break;
        }
    }

    if (DidPlayersWin) return EWhoWins::Players;

    return EWhoWins::Nobody;
}