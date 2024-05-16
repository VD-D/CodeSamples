// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerConfiguration.generated.h"

class UBehaviorTree;
class UCharacterPanel;
class UCharacterSelectContainerWidget;
class UDataTable;
class UMaterialInterface;

/**
 * Holds configuration values for the autobattler game.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerConfiguration : public UDataAsset
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// GRID
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Size of new grid. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Grid")
	float GridSize;

	/* How large the deployment grid should be. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Grid")
	FIntPair GridDimensions;

	/* Grid decal for visualisation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Grid")
	UMaterialInterface* GridDecal;

/////////////////////////////////////////////////////////////////////////////////
//// PLAYER
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Must have row struct type FAutobattlerPlayerAbility. List of all available player abilities. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Player Ability")
	const UDataTable* PlayerAbilityDatatable;

/////////////////////////////////////////////////////////////////////////////////
//// CHARACTERS
/////////////////////////////////////////////////////////////////////////////////
	/* Data table defining all available (player and enemy) characters. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Characters")
	const UDataTable* AllCharactersDataTable;

	// TODO: Add modifiers datatable
	/* Data table defining all available (player and enemy) modifiers. */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Characters")
	//const UDataTable* AllModifiersDataTable;

	/* Custom depth stencil applied to all characters (used for outlines). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Characters")
	int32 CustomDepthStencil;

	/* Rotation to apply to the preview character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Characters")
	FRotator PreviewCharacterRotation;

/////////////////////////////////////////////////////////////////////////////////
//// UI
/////////////////////////////////////////////////////////////////////////////////
	/* Widget class to use for the character panel widget. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|UI", meta = (BlueprintBaseOnly))
	TSubclassOf<UCharacterPanel> CharacterPanelWidgetClass;

	/* Widget class to use for the character selection container widget. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|UI", meta = (BlueprintBaseOnly))
	TSubclassOf<UCharacterSelectContainerWidget> CharacterSelectContainerWidgetClass;

/////////////////////////////////////////////////////////////////////////////////
//// LOCALISATION
/////////////////////////////////////////////////////////////////////////////////
	/* Exists for localisation purposes. Name of Resistance type enum translated to user-friendly text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Localisation")
	TMap<EResistanceType, FText> ResistanceTextMap;

	/* Exists for localisation purposes. Name of Damage type enum translated to user-friendly text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Localisation")
	TMap<EDamageType, FText> DamageTextMap;

/////////////////////////////////////////////////////////////////////////////////
//// GAME
/////////////////////////////////////////////////////////////////////////////////
	/* How a damage type is resisted/amplified by a given container. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Game")
	TMap<EDamageType, FResistanceContainer> DamageModifierMap;

	/* How far away a projectile skill or attack needs to be from its target to be considered a "hit." */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Game")
	float ProjectileMinDistanceToHit;

	/* How often poison ticks (in seconds). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Game", meta = (ClampMin = "1"))
	float PoisonTickRate;

	/* How much strength poison loses per tick. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Game", meta = (ClampMin = "1"))
	float PoisonStrengthReductionRate;

/////////////////////////////////////////////////////////////////////////////////
//// AI
/////////////////////////////////////////////////////////////////////////////////
	/* Datatable containing possible formations used by the AI. Should have row type AIFormation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|AI")
	const UDataTable* AIFormationsDatatable;

	/* Flag which will make the AI use the BT over a native AIUpdate function. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|AI")
	bool UseBehaviorTree;

	/* Behavior tree used by Autobattler Characters. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|AI", meta = (EditCondition = "UseBehaviorTree"))
	UBehaviorTree* AutobattlerBehaviorTree;

	/* Default rotation of enemy characters when spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|AI")
	FRotator EnemyCharacterRotation;

/////////////////////////////////////////////////////////////////////////////////
//// TRACES
/////////////////////////////////////////////////////////////////////////////////
	/* Which collision channel to use when checking for occupancy. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Traces")
	TEnumAsByte<ECollisionChannel> GridOccupancyCollisionChannel;

/////////////////////////////////////////////////////////////////////////////////
//// DEBUG
/////////////////////////////////////////////////////////////////////////////////
	/* Datatable containing battle configurations used for debugging. Should have row type DebugBattleConfiguration. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Configuration|Debug")
	const UDataTable* DebugBattleConfigurations;



/////////////////////////////////////////////////////////////////////////////////
//// UTILITY
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Default constructor.
	 */
	UAutobattlerConfiguration();

	/**
	 * Static getter for the configuration asset. Will only return a valid configuration
	 * if one has been definined in the autobattler settings and has been loaded.
	 * @return The configuration asset, if defined in settings and loaded.
	 */
	static const UAutobattlerConfiguration* GetConfigurationAsset(const UObject* WorldContextObject);
};
