// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/AutobattlerEnums.h"
#include "WinConditionBase.generated.h"

class AAutobattlerCharacter;

/**
 * Class which returns an implementation of who wins. By default, one side wins if all enemy
 * characters are dead, but the method can be overriden for specific scenarios.
 */
UCLASS(BlueprintAble)
class AUTOBATTLERPLUGIN_API UWinConditionBase : public UObject
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIN CONDITION API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Checks to see if either side has won. By default, will return Enemy if all player (client & server) characters are dead or there are none left,
	 * Players if all enemy characters are dead or there are none left and Nobody if both sides are still fighting.
	 * @param ParticipatingCharacters Characters currently on the battlefield.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Win Condition")
	EWhoWins CheckWinCondition(const TArray<AAutobattlerCharacter*>& ParticipatingCharacters);
	virtual EWhoWins CheckWinCondition_Implementation(const TArray<AAutobattlerCharacter*>& ParticipatingCharacters);
};
