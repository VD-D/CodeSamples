// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Animation/EndRessurect.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Utility/AutobattlerFunctionLibrary.h"

void UEndRessurect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!IsValid(MeshComp))
    {
        UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [Notify] Does not have valid mesh comp!"), *GetName()));
        return;
    }

    if (AAutobattlerCharacter* OwnerCharacter = Cast<AAutobattlerCharacter>(MeshComp->GetOwner()))
    {
        if (!OwnerCharacter->HasAuthority()) return;
        OwnerCharacter->EndRessurect();
    }
    else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [Notify] Owner is not an Autobattler Character!"), *GetName()));
}
