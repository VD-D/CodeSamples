// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AutobattlerEnums.generated.h"

/* Enum defining the possibile entities in the autobattler game. */
UENUM(BlueprintType)
enum class EEntity : uint8
{
	AI        UMETA(DisplayName = "AI"),
	PlayerOne UMETA(DisplayName = "PlayerOne"),
	PlayerTwo UMETA(DisplayName = "PlayerTwo"),
	Count     UMETA(Hidden)
};

/* Enum defining the possible identities in the autobattler game. */
UENUM(BlueprintType)
enum class EIdentity : uint8
{
	Player1 UMETA(DisplayName = "Player One"),
	Player2 UMETA(DisplayName = "Player Two"),
	Enemy   UMETA(DisplayName = "Enemy"),
	Count   UMETA(Hidden)
};

/* What happens when a skill is triggered. */
UENUM(BlueprintType)
enum class ESkillTriggerEnd : uint8
{
	Instant    UMETA(DisplayName = "Instant"),
	Projectile UMETA(DisplayName = "Projectile"),
	Effect     UMETA(DisplayName = "Effect"),
	Count      UMETA(Hidden)
};

/* Damage types in the autobattler game. */
UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Pierce   UMETA(DisplayName = "Pierce"),
	Bludgeon UMETA(DisplayName = "Bludgeon"),
	Slash    UMETA(DisplayName = "Slash"),
	Impact   UMETA(DisplayName = "Impact"),
	Count    UMETA(Hidden)
};

/* Resistance types in the autobattler game. */
UENUM(BlueprintType)
enum class EResistanceType : uint8
{
	Parry     UMETA(DisplayName = "Parry"),
	Fortified UMETA(DisplayName = "Fortified"),
	Absorb    UMETA(DisplayName = "Absorb"),
	Swift     UMETA(DisplayName = "Swift"),
	Count     UMETA(Hidden)
};

/* What actions a autobattler character can undertake. */
UENUM(BlueprintType)
enum class EActionType : uint8
{
	Idle       UMETA(DisplayName = "Idle"),
	Attacking  UMETA(DisplayName = "Attacking"),
	UsingSkill UMETA(DisplayName = "Using Skill"),
	Dead       UMETA(DisplayName = "Dead"),
	Victory    UMETA(DisplayName = "Victory"),
	Ressurect  UMETA(DisplayName = "Ressurect"),
	Count      UMETA(Hidden)
};

/* Ability types (whether an ability is a skill or attack). */
UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	Attack UMETA(DisplayName = "Attack"),
	Skill  UMETA(DisplayName = "Skill"),
	Count  UMETA(Hidden)
};

/* What phase of the game we are at. */
UENUM(BlueprintType)
enum class EAutobattlerPhase : uint8
{
	Setup UMETA(DisplayName = "Setup"),
	Fight UMETA(DisplayName = "Fight"),
	End   UMETA(DisplayName = "End"),
	Count UMETA(Hidden)
};

/* Filter types for skill and attack targeting. */
UENUM(BlueprintType)
enum class EAbilityFilterType : uint8
{
	Self          UMETA(DisplayName = "Self"),
	SelfAndAllies UMETA(DisplayName = "Self and Allies"),
	AlliesOnly    UMETA(DisplayName = "Allies Only"),
	Enemies       UMETA(DisplayName = "Enemies"),
	All           UMETA(DisplayName = "All"),
	Count         UMETA(Hidden)
};

/* States for a unit currently fighting. */
UENUM(BlueprintType)
enum class EFightingUnitState : uint8
{
	Idle           UMETA(DisplayName = "Idle"),
	CheckingRange  UMETA(DisplayName = "Checking Range"),
	GettingInRange UMETA(DisplayName = "Getting in Range"),
	Attacking      UMETA(DisplayName = "Attacking"),
	UsingSkill     UMETA(DisplayName = "Using Skill"),
	Count          UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ESkillTargetingMode : uint8
{
	Location UMETA(DisplayName = "Location"), 
	Actor    UMETA(DisplayName = "Actor"),
	None     UMETA(DisplayName = "None"),
	Count    UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ESkillCollisionType : uint8
{
	Single UMETA(DisplayName = "Single"),
	Sphere UMETA(DisplayName = "Sphere"), 
	Box    UMETA(DisplayName = "Box"),
	Count  UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWhoWins : uint8
{
	Enemy   UMETA(DisplayName = "Enemy"),
	Players UMETA(DisplayName = "Players"),
	Nobody  UMETA(DisplayName = "Nobody")
};

UENUM(BlueprintType)
enum class EAutobattlerStatType : uint8
{
	MovementSpeed      UMETA(DisplayName = "Movement Speed"),
	AttackSpeed        UMETA(DisplayName = "Attack Speed"),
	SkillSpeed         UMETA(DisplayName = "Skill Speed"),
	CriticalChance     UMETA(DisplayName = "Critical Chance"),
	CriticalMultiplier UMETA(DisplayName = "Critical Multiplier"),
	MaxHealth          UMETA(DisplayName = "Max Health"),
	AttackRange        UMETA(DisplayName = "Attack Range"),
	AbilityRange       UMETA(DisplayName = "Ability Range"),
	Count              UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EAutobattlerDiscreteType : uint8
{
	ResistanceType UMETA(DisplayName = "Resistance Type"),
	DamageType     UMETA(DisplayName = "Damage Type"),
	Count          UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EPlaceReason : uint8
{
	InvalidLocation    UMETA(DisplayName = "Invalid Location"),
	OverBudget         UMETA(DisplayName = "Over Budget"),
	CannotPlaceOnIndex UMETA(DisplayName = "Cannot Place on Grid Index"),
	CanBePlaced        UMETA(DisplayName = "Can Be Placed"),
	Count              UMETA(Hidden)
};

/**
 * Shared enums in the autobattler.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerEnums : public UObject
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// UTILITY
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Creates string representation of damage type.
	 * @param DamageType Damage type to conver to string.
	 * @return String representation of damage type.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Enumerations")
	static FString DamageTypeToString(EDamageType DamageType);

	/**
	 * Creates string representation of resistance type.
	 * @param ResistanceType Resistance type to conver to string.
	 * @return String representation of resistance type.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Enumerations")
	static FString ResistanceTypeToString(EResistanceType ResistanceType);
};
