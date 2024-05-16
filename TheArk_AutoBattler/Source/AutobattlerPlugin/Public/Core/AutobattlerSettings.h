// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AutobattlerSettings.generated.h"

class UAutobattlerConfiguration;

/**
 * Developer settings for the autobattler.
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName="Autobattler Settings"))
class AUTOBATTLERPLUGIN_API UAutobattlerSettings : public UDeveloperSettings
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// DEVELOPER SETTINGS
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Collision channel used for mouse traces. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	TEnumAsByte<ECollisionChannel> MouseCollisionChannel = TEnumAsByte<ECollisionChannel>(ECollisionChannel::ECC_Visibility);

	/* Collision channel used for collision between characters. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	TEnumAsByte<ECollisionChannel> CharacterCollisionChannel = TEnumAsByte<ECollisionChannel>(ECollisionChannel::ECC_Pawn);

	/* Input name for primary actions, i.e. placing characters. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	FName PrimaryActionInputName = FName("PrimaryAction");

	/* Input name for secondary actions, i.e. cancelling placement. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	FName SecondaryActionInputName = FName("SecondaryAction");

	/* This asset holds configuration values, which are loaded when the game is initialised. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	TSoftObjectPtr<UAutobattlerConfiguration> ConfigurationAsset;

	/* Blackboard key name of the key which corresponds to the current game phase (should have enum object type, filtered by AutobattlerPhase). */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blackboard Key Names")
	FName GamePhaseBlackboardKey = FName("GamePhase");

	/* Blackboard key name of the key which corresponds to the next action (should have enum type, filtered by ActionType). */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blackboard Key Names")
	FName ActionBlackboardKey = FName("NextAction");

	/* Blackboard key name of the key which corresponds to whether or not the next ability's target is in range (should have bool type) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blackboard Key Names")
	FName IsInRangeBlackboardKey = FName("IsInRange");
};
