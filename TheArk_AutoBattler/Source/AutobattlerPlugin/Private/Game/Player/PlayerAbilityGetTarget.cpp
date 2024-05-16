// Fill out your copyright notice in the Description page of Project Settings.

/* Class header. */
#include "Game/Player/PlayerAbilityGetTarget.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"

void UPlayerAbilityGetTarget::PlayerGetTargets_Implementation(UAutobattlerControllerComponent* PlayerContext, TArray<AAutobattlerCharacter*>& ChosenTargets)
{
    ChosenTargets.Empty();
}

void UPlayerAbilityGetTarget::GetAllowedEntities(UAutobattlerControllerComponent* PlayerContext, EPlayerAbilityTarget ValidTargetTypes, TArray<EEntity>& AllowedEntities)
{
    if (!IsValid(PlayerContext))
    {
        AllowedEntities.Empty();
        return;
    }

    EEntity OwnerEntity = PlayerContext->GetIdentity();
    AllowedEntities.Empty(3);

    if (ValidTargetTypes == EPlayerAbilityTarget::OwnUnits) AllowedEntities.Emplace(OwnerEntity); // Case where we are targeting own units.
    else if (ValidTargetTypes == EPlayerAbilityTarget::OtherPlayer) AllowedEntities.Emplace(OwnerEntity == EEntity::PlayerOne ? EEntity::PlayerTwo : EEntity::PlayerOne); // Case where we add other player's units.
    else if (ValidTargetTypes == EPlayerAbilityTarget::Allied)
    {
        AllowedEntities.Emplace(EEntity::PlayerOne);
        AllowedEntities.Emplace(EEntity::PlayerTwo); // Case where we add both P1 and P2 units.
    }
    else if (ValidTargetTypes == EPlayerAbilityTarget::Enemy) AllowedEntities.Emplace(EEntity::AI); // Case where we target enemy units.
    else if (ValidTargetTypes == EPlayerAbilityTarget::All)
    {
        AllowedEntities.Emplace(EEntity::PlayerOne);
        AllowedEntities.Emplace(EEntity::PlayerTwo);
        AllowedEntities.Emplace(EEntity::AI);
    }
}
