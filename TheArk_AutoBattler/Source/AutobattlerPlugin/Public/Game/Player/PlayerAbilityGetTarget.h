// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/AutobattlerEnums.h"
#include "PlayerAbilityGetTarget.generated.h"

class AAutobattlerCharacter;
class UAutobattlerControllerComponent;

/* Various possible targets for a player autobattler ability. */
UENUM(BlueprintType)
enum class EPlayerAbilityTarget : uint8
{
	OwnUnits UMETA(DisplayName = "Own Units"),
	OtherPlayer UMETA(DisplayName = "Allied Player Units"),
	Allied UMETA(DisplayName = "Own & Allied Units"),
	Enemy UMETA(DisplayName = "Enemy Units"),
	All UMETA(DisplayName = "All Units"),
	Count UMETA(Hidden)
};

/* Possible types of player targeting. */
UENUM(BlueprintType)
enum class EPlayerTargetType : uint8
{
	NoManualTargeting UMETA(DisplayName = "No Manual Targeting"),
	TargetCharacter UMETA(DisplayName = "Single Target Character"),
	TargetCharacterAOE UMETA(DisplayName = "Target Character + AoE"),
	Location UMETA(DisplayName = "Target Location"),
	Count UMETA(Hidden)
};

/* Possible types of player targeting. */
UENUM(BlueprintType)
enum class EAoETargetType : uint8
{
	Box UMETA(DisplayName = "Box"),
	Sphere UMETA(DisplayName = "Sphere"),
	Count UMETA(Hidden)
};

/**
 * Object which should return all characters or points which can be targeted by a player's ability.
 */
UCLASS(Blueprintable, Abstract, DefaultToInstanced, EditInlineNew)
class AUTOBATTLERPLUGIN_API UPlayerAbilityGetTarget : public UObject
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Function which should use params to get the appropriate targets. Default implementation empties ChosenTargets.
	 * @param PlayerContext Which is the player executing the ability.
	 * @param ChosenTargets (OUT) Targets chosen for the skill.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler")
	void PlayerGetTargets(UAutobattlerControllerComponent* PlayerContext, TArray<AAutobattlerCharacter*>& ChosenTargets);
	virtual void PlayerGetTargets_Implementation(UAutobattlerControllerComponent* PlayerContext, TArray<AAutobattlerCharacter*>& ChosenTargets);

	/**
	 * Utility function which gets all entity types which can be targeted based on player target.
	 * E.g. For ValidTargetTypes = OwnUnits and PlayerContext->Owner = PlayerOne, AllowedEntities will contain PlayerOne.
	 * @param PlayerContext Used to get owning entity.
	 * @param ValidTargetTypes What the valid target types are.
	 * @param AllowedEntities (OUT) Which entities are valid targets.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	static void GetAllowedEntities(UAutobattlerControllerComponent* PlayerContext, EPlayerAbilityTarget ValidTargetTypes, TArray<EEntity>& AllowedEntities);

/////////////////////////////////////////////////////////////////////////////////
//// CONFIG
/////////////////////////////////////////////////////////////////////////////////
public:
	/* What the target of the ability should be. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlayerAbilityTarget WhatToTarget;
};
