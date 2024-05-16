// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/GetEnemyPlacement.h"

void UGetEnemyPlacement::GetEnemyPlacements_Implementation(const TArray<FIntPair>& ValidLocations, int32 NumEnemiesToPlace, TArray<FIntPair>& OutPlacements)
{
    TArray<FIntPair> LocationsToPickFrom = ValidLocations;
    int32 ClampedEnemiesToPlace = NumEnemiesToPlace < 1 ? 1 : NumEnemiesToPlace;
    for (int32 i = 0; i < ClampedEnemiesToPlace; i++)
    {
        if (LocationsToPickFrom.Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, LocationsToPickFrom.Num() - 1);
            OutPlacements.Emplace(LocationsToPickFrom[RandomIndex]);
            LocationsToPickFrom.RemoveAt(RandomIndex);
        }
    }
}
