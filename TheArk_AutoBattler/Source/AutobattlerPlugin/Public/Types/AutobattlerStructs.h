// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/AutobattlerPlayerAbilityAsset.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "Types/AutobattlerEnums.h"
#include "AutobattlerStructs.generated.h"

class AAutobattlerCharacter;
class AAutobattlerPreviewCharacter;
class APlayerExecuteAbility;
class UAutobattlerAbility;
class UAutobattlerAttack;
class UAutobattlerPlayerAbilityAsset;
class UEnvQuery;
class UMaterialInterface;
class UPlayerAbilityGetTarget;
class USkillImplementation;
class UTexture2D;

/* X/Y coordinate structure, used by the autobattler grid. */
USTRUCT(BlueprintType)
struct FIntPair
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y;

	FIntPair()
	{
		X = 0;
		Y = 0;
	}

	FIntPair(int32 NewX, int32 NewY)
	{
		X = NewX;
		Y = NewY;
	}

	FORCEINLINE bool operator==(const FIntPair& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	FORCEINLINE bool operator!=(const FIntPair& Other) const
	{
		return X != Other.X || Y != Other.Y;
	}

	FORCEINLINE FIntPair operator+(const FIntPair& Other)
	{
		return FIntPair(X + Other.X, Y + Other.Y);
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("%d:%d"), X, Y);
	}
};

/* This allows FIntPairs to be used as keys in a TMap. */
#if UE_BUILD_DEBUG
	uint32 GetTypeHash(const FIntPair& Pair);
#else
FORCEINLINE uint32 GetTypeHash(const FIntPair& Pair)
{
    return FCrc::MemCrc32(&Pair, sizeof(FIntPair));
}
#endif

USTRUCT(BlueprintType)
struct FRandomRange
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max;

	FRandomRange()
	{
		Min = 0.0f;
		Max = 1.0f;
	}

	FRandomRange(float NewMin, float NewMax)
	{
		Min = NewMin;
		Max = NewMax;
	}

	float GetRandomValueInRange()
	{
		return FMath::RandRange(Min, Max);
	}

	float GetRandomValueInRange(const FRandomStream& Stream)
	{
		return Stream.FRandRange(Min, Max);
	}
};

/* Struct defining character modifiers. */
USTRUCT(BlueprintType)
struct FMaterialContainer
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInterface*> Materials;
};

USTRUCT(BlueprintType)
struct FResistanceContainer
{
	GENERATED_BODY()
public:
	/* Intended to be matched against a damage type. How resistances work against that damage type. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResistanceType, float> Resistances;

	FResistanceContainer() { }

	FResistanceContainer(const TMap<EResistanceType, float>& NewResistances)
	{
		Resistances = NewResistances;
	}
};

/* Struct defining an autobattler character without modifiers. */
USTRUCT(BlueprintType)
struct FAutobattlerCharacterDefinition : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Cosmetic name of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	FText CharacterName;

	/* Cosmetic description of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	FText CharacterDescription;

	/* How much health this character has. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	float BaseHealth;

	/* How fast the character moves. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	float MovementSpeed;

	/* Default damage type of this character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	EDamageType BaseDamageType;

	/* Default resistance type of this character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	EResistanceType BaseResistanceType;

	/* Chance of critical attack. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character", meta = (ClampMin="0", ClampMax="1"))
	float CriticalChance;

	/* Scale of critical attack damage. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character", meta = (ClampMin="1"))
	float CriticalMultiplier;

	/* Icon of this character in the UI. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	UTexture2D* CharacterIcon;

	/* Character class to use for this character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	TSubclassOf<AAutobattlerCharacter> CharacterClass;

	/* Preview ("ghost") character shown when pending deployment. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	TSubclassOf<AAutobattlerPreviewCharacter> PreviewCharacterClass;

	/* Additional character offset, typically used for offsetting larger characters so they are not centered on the grid. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	FVector DeploymentOffset;

	/* Attack implementation of this character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	const UAutobattlerAttack* AttackImplementation;

	/* Ability implementation of this character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	const UAutobattlerAbility* AbilityImplementation;

	/* How "expensive" this character is with regards to the budget. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	int32 BudgetCost;

	/* Relevant only if this character is deployed by an AI. Tags at the beginning of the array are more
	preferred than those which come after it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	FName PreferedDeploymentLocation;

	/* Relevant only if this character is deployed by an AI. Tags which determine whether this character is
	filtered when the AI wants to deploy it (characters on whitelist will be chosen, but then removed if they have a
	tag on the blacklist). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	TArray<FName> BlackWhiteListTags;

	/* EQS query for pathing around this character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	UEnvQuery* PathAroundQuery;

	/* EQS query for finding points around character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	UEnvQuery* SurroundingPointsQuery;
};

/* Struct defining character modifiers. */
USTRUCT(BlueprintType)
struct FModifier : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* In the future, this struct will hold a modifier to apply to the character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Character")
	bool DummyValue;
};

/* Container for defining a character and his/her modifiers. */
USTRUCT(BlueprintType)
struct FCharacterListing
{
	GENERATED_BODY()
public:
	/* Name of the character from AutobattlerConfiguration->AllCharactersDataTable. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CharacterRowName;

	/* All modifiers to be applied to the character from AllCharactersDataTable->AllModifiersDataTable. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> ModifierTableRowNames;

	FCharacterListing()
	{
		CharacterRowName = FName("None");
	}

	FCharacterListing(const FName& NewCharacterRowName)
	{
		CharacterRowName = NewCharacterRowName;
	}

	FCharacterListing(const FName& NewCharacterRowName, const TArray<FName>& NewModifierTableRowNames)
	{
		CharacterRowName = NewCharacterRowName;
		ModifierTableRowNames = NewModifierTableRowNames;
	}
};

/* Struct defining what characters an entity (player or AI) can place,
where and how many. */
USTRUCT(BlueprintType)
struct FEntityConfiguration
{
	GENERATED_BODY()
public:
	/* All characters which player can place. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCharacterListing> AvailableCharacters;

	/* Grid location which player can place on. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntPair> ValidGridDeploymentLocations;

	/* Max number of characters player can deploy. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCharactersToDeploy;
};

/* Struct for storing targeting properties. */
USTRUCT(BlueprintType)
struct FAbilityTargetingProperties
{
	GENERATED_BODY()
public:
	/* Range of the ability. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Range;

	/* The target character of the ability. Relevant only if TargetingMode == Actor. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	AAutobattlerCharacter* TargetCharacter;

	/* The target character of the ability. Relevant only if TargetingMode == Location. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVector TargetLocation;

	/* The targeting mode of this ability. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ESkillTargetingMode	TargetingMode;

	FAbilityTargetingProperties()
	{
		TargetCharacter = nullptr;
		TargetLocation = FVector::ZeroVector;
		TargetingMode = ESkillTargetingMode::None;
	}

	FAbilityTargetingProperties(AAutobattlerCharacter* NewCharacter)
	{
		TargetCharacter = NewCharacter;
		TargetLocation = FVector::ZeroVector;
		TargetingMode = ESkillTargetingMode::Actor;
	}

	FAbilityTargetingProperties(const FVector& NewLocation)
	{
		TargetCharacter = nullptr;
		TargetLocation = NewLocation;
		TargetingMode = ESkillTargetingMode::Location;
	}
};

/* Character stats which need to be replicated for cosmetic purposes. */
USTRUCT(BlueprintType)
struct FCharacterBaseStats
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float BaseHealth;

	UPROPERTY()
	float BaseMovementSpeed;

	UPROPERTY()
	float BaseCriticalChance;

	UPROPERTY()
	float BaseCriticalMultiplier;

	UPROPERTY()
	FText BaseDamageType;

	UPROPERTY()
	FText BaseResistanceType;
};

/* Used to generated AI formations. */
USTRUCT(BlueprintType)
struct FAIFormation : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Contains FIntPairs (deployment indexes) mapped to names, 
	i.e. tags which define what "type" of field it is (and subsequently what unit perfers to be deployed in it). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	TMap<FIntPair, FName> FormationMap;
};

/* This exists to define how a soldier should be generated in a debug battle. */
USTRUCT(BlueprintType)
struct FDebugSoldierConfiguration : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Listing from which to generate this character from. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	FCharacterListing SoldierListing;

	/* Whether this soldier should already start on the battlefield. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	bool StartDeployed = true;

	/* Which index this soldier should be deployed on. Adds this index to allowed deployement indicies for owner. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler", meta = (EditCondition = "StartDeployed"))
	FIntPair DeploymentIndex;
};

/* This exists so we can map an entity to a debug soldier configartion array. */
USTRUCT(BlueprintType)
struct FDebugSoldierContainer : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* All soldiers belonging to an entity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	TArray<FDebugSoldierConfiguration> SoldierConfigurations;
};

/* Configuration for generating a debug battle (used for balancing and design purposes). */
USTRUCT(BlueprintType)
struct FDebugBattleConfiguration : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Owners to the soldiers specifically used for this debug battle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	TMap<EEntity, FDebugSoldierContainer> EntityConfigurations;

	/* When generating a battle from this configurations, should previous soldiers to removed?
	(WARNING: If true, removes ALL soldiers from all barracks, including the AI) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	bool ClearAutobattlerState;

	/* Formation used by the AI for this battle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	FName AIFormationRowName;

	/* How many enemies should be generated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	int32 AIBudget;

	/* Whitelist passed to GenerateAIFormation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	TArray<FName> AIFormationWhitelist;

	/* Whitelist passed to GenerateAIFormation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	TArray<FName> AIFormationBlacklist;
};

/* Used to generate a random number of autobattler characters. */
USTRUCT(BlueprintType)
struct FCharacterRandomiserParameters
{
	GENERATED_BODY()
public:
	/* Row name of character. Row names are taken from All Characters datable, defined in Autobattler Config. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterRowName;

	/* How many of these characters to add to barracks. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
	int32 Quantity = 3;

	/* The chance that this character will be included, on 0.001 to 1 scale (1 = guaranteed include). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.001", ClampMax = "1"))
	float Chance = 1.0f;

	/* If true, then the chance to generate a character is rolled per-character (e.g. for Quantity = 4 and Chance = 0.75
	we could expect 3 characters to be generated, since statistically 75% of 4 is 3).
	If false, then the chance to generate a character is rolled once. (e.g. for Quantity = 4, Chance = 0.75, there is a
	75% we will get all 4 and a 25% chance we will get none).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PerCharacterChance = true;
};

/* Used to generate random tags for AI whitelist and blacklist when generating formation. */
USTRUCT(BlueprintType)
struct FTagRandomiserParameters
{
	GENERATED_BODY()
public:
	/* Actual tag. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Tag;

	/* The chance that this tag will be included, on 0.001 to 1 scale (1 = guaranteed include). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.001", ClampMax = "1"))
	float Chance = 1.0f;
};

/* Configuration for populating barracks and generating an AI formation based on it. */
USTRUCT(BlueprintType)
struct FAutobattlerAIFormation : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Whether to clear previous AI state when generating this formation (removes all deployed AI characters and empties AI barracks). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	bool ClearPreviousAIState = true;

	/* The AI is limited by budget, like players are. Note this sets the budget to this value (does not add on to previous budget). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	int32 AIBudget = 9999;

	/* Formation rowname from formations datatable defined in Autobattler Configuration. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	FName FormationRowName;

	/* Map of rowname to quantity of new AI soldiers to be added to barracks. Row names are taken from All Characters datable, defined in Autobattler Config. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	TArray<FCharacterRandomiserParameters> NewAICharacters;

	/* Whitelist to be used when generating formation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	TArray<FTagRandomiserParameters> WhitelistTags;

	/* Blacklist to be used when generating formation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	TArray<FTagRandomiserParameters> BlacklistTags;
};

/* Struct defining a player-useable ability. */
USTRUCT(BlueprintType)
struct FAutobattlerPlayerAbility : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Name of the player skill. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	FText PlayerSkillName;

	/* User friendly description of the skill. Rich text supported. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler", meta = (MultiLine="true"))
	FText PlayerSkillDescription;

	/* Icon representing player ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	UTexture2D* Icon;

	/* Data asset holding implementation of player ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler")
	const UAutobattlerPlayerAbilityAsset* AutobattlerPlayerAbilityImplementation;

	/* How long before this skill can be used again. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler", meta = (ClampMin = "1.0"))
	float Cooldown = 1.0f;
};

/* Struct used only internally to replicate ID and cosmetic skill data. */
USTRUCT()
struct FAutobattlerPlayerAbilityCosmetic
{
	GENERATED_BODY()
public:
	/* Unique ID of this ability. */
	UPROPERTY()
	FName ID;

	/* Name of the player skill. */
	UPROPERTY()
	FText PlayerSkillName;

	/* User friendly description of the skill. */
	UPROPERTY()
	FText PlayerSkillDescription;

	/* Icon representing player ability. */
	UPROPERTY()
	UTexture2D* Icon;

	FAutobattlerPlayerAbilityCosmetic()
	{
		ID = FName("None");
		PlayerSkillName = FText::GetEmpty();
		PlayerSkillDescription = FText::GetEmpty();
		Icon = nullptr;
	}

	FAutobattlerPlayerAbilityCosmetic(const FName& NewID, FAutobattlerPlayerAbility* Ability)
	{
		ID = NewID;
		PlayerSkillName = Ability != nullptr ? Ability->PlayerSkillName : FText::GetEmpty();
		PlayerSkillDescription = Ability != nullptr ? Ability->PlayerSkillDescription : FText::GetEmpty();
		Icon = Ability != nullptr ? Ability->Icon : nullptr;
	}
};

/* Struct used only internally to hold an ability implementation. */
USTRUCT()
struct FAutobattlerPlayerAbilityData
{
	GENERATED_BODY()
public:
	/* Unique ID of this ability. */
	UPROPERTY()
	FName ID;

	/* Class for executing ability, which should vary depending on special effects. */
	UPROPERTY()
	TSubclassOf<APlayerExecuteAbility> PlayerExecuteAbilityClass;

	/* Actual implementation of the skills. Note that the skill owner is always nullptr */
	UPROPERTY()
	TArray<USkillImplementation*> SkillEffects;

	/* What targets these skills can possibly affect (noting that they are still limited by targeting options of
	skills themselves). */
	UPROPERTY()
	UPlayerAbilityGetTarget* GetTargetImplementation;

	/* How long before this skill can be used again. */
	UPROPERTY()
	float Cooldown;

	FAutobattlerPlayerAbilityData()
	{
		ID = FName("None");
		GetTargetImplementation = nullptr;
	}

	FAutobattlerPlayerAbilityData(const FName& NewID, FAutobattlerPlayerAbility* Ability)
	{
		if (Ability != nullptr)
		{
			if (Ability->AutobattlerPlayerAbilityImplementation != nullptr)
			{
				ID = NewID;
				GetTargetImplementation = Ability->AutobattlerPlayerAbilityImplementation->GetTargetImplementation;
				PlayerExecuteAbilityClass = Ability->AutobattlerPlayerAbilityImplementation->PlayerExecuteAbilityClass;
				Cooldown = Ability->Cooldown;
				SkillEffects = Ability->AutobattlerPlayerAbilityImplementation->SkillEffects;
			}
			else
			{
				ID = FName("None");
				Cooldown = 1.0f;
			}
		}
		else
		{
			ID = FName("None");
			Cooldown = 1.0f;
		}
	}
};

/**
 * Structs shared between classes in the autobattler plugin.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerStructs : public UObject
{
	GENERATED_BODY()
};
