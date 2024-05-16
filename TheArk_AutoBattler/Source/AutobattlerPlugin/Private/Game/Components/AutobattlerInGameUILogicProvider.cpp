// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Components/AutobattlerInGameUILogicProvider.h"

UAutobattlerInGameUILogicProvider::UAutobattlerInGameUILogicProvider()
{
	SetIsReplicatedByDefault(true);
}

bool UAutobattlerInGameUILogicProvider::ShouldShowInGameUIElements_Implementation()
{
	return true;
}
