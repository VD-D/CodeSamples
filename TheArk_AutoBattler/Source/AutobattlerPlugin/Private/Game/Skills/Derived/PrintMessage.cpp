// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Skills/Derived/PrintMessage.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"
#include "Utility/AutobattlerFunctionLibrary.h"

void UPrintMessage::ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation)
{
    FString SkillOwnerName = IsValid(SkillOwner) ? SkillOwner->GetName() : FString("Unknown Owner");
    UAutobattlerFunctionLibrary::PrintMessage(
        FString::Printf(TEXT("%s : %s"), *SkillOwnerName, *Message),
        EVerbosityLevel::Log,
        true,
        false,
        -1,
        Duration
    );
}
