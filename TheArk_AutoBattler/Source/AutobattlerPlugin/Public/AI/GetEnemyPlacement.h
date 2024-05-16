// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/AutobattlerStructs.h"
#include "GetEnemyPlacement.generated.h"

/**
 * Object which holds an algorithm responsible for generating enemy placements.
 */
UCLASS(Blueprintable)
class AUTOBATTLERPLUGIN_API UGetEnemyPlacement : public UObject
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// ENEMY PLACEMENT API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Algorithm for getting enemy placements. Default algorithm returns completely random locations.
	 * @param ValidLocations Locations which the AI is allowed to place its character's on.
	 * @param NumEnemiesToPlace The number of character's the AI intends to place.
	 * @param OutPlacements Locations to place on.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Get Enemy Placement")
	void GetEnemyPlacements(const TArray<FIntPair>& ValidLocations, int32 NumEnemiesToPlace, TArray<FIntPair>& OutPlacements);
	virtual void GetEnemyPlacements_Implementation(const TArray<FIntPair>& ValidLocations, int32 NumEnemiesToPlace, TArray<FIntPair>& OutPlacements);
};
