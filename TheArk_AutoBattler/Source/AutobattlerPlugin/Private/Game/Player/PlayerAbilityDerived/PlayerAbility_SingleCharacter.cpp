// Fill out your copyright notice in the Description page of Project Settings.

/* Class header. */
#include "Game/Player/PlayerAbilityDerived/PlayerAbility_SingleCharacter.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"
#include "Game/Units/AutobattlerCharacter.h"

bool UPlayerAbility_SingleCharacter::IsTargetValid_Implementation(UAutobattlerControllerComponent* SkillOwner, const FHitResult& MouseHitResult) const
{
    if (MouseHitResult.bBlockingHit)
    {
        if (IsValid(MouseHitResult.GetActor()))
        {
            if (AAutobattlerCharacter* Character = Cast<AAutobattlerCharacter>(MouseHitResult.GetActor()))
            {
                TArray<EEntity> AllowedTargets;
                UPlayerAbilityGetTarget::GetAllowedEntities(SkillOwner, WhatToTarget, AllowedTargets);

                return AllowedTargets.Contains(Character->GetOwnerIdentity());
            }
        }
    }

    return false;
}
