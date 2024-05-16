// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Skills/Derived/ApplyPoison.h"

/* Autobattler includes. */
#include "Game/Components/PoisonComponent.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Utility/AutobattlerFunctionLibrary.h"

void UApplyPoison::ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation)
{
    if (IsValid(Target))
    {
        if (UPoisonComponent* PoisonComponent = UAutobattlerFunctionLibrary::GetFirstComponent<UPoisonComponent>(Target))
        {
            PoisonComponent->ApplyPoison(MinMaxPoison.GetRandomValueInRange());
        }
        else
        {
            UPoisonComponent* NewPoisonComponent = Cast<UPoisonComponent>(Target->AddComponentByClass(UPoisonComponent::StaticClass(), false, FTransform::Identity, false));
            if (IsValid(NewPoisonComponent)) NewPoisonComponent->ApplyPoison(MinMaxPoison.GetRandomValueInRange());
        }
    }
}
