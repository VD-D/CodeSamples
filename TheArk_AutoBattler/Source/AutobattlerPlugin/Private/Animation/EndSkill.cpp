// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Animation/EndSkill.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Utility/AutobattlerFunctionLibrary.h"

void UEndSkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!IsValid(MeshComp))
    {
        UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [Notify] Does not have valid mesh comp!"), *GetName()));
        return;
    }

    if (AAutobattlerCharacter* OwnerCharacter = Cast<AAutobattlerCharacter>(MeshComp->GetOwner()))
    {
        if (!OwnerCharacter->HasAuthority()) return;
        if (AAutobattlerAIController* AIController = Cast<AAutobattlerAIController>(OwnerCharacter->GetController()))
        {
            AIController->EndSkill();
        }
        else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [Notify] Owner is not possessed by Autobattler AI Controller!"), *GetName()));
    }
    else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [Notify] Owner is not an Autobattler Character!"), *GetName()));
}