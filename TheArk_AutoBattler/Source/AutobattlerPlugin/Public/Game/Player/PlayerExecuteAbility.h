// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/AutobattlerStructs.h"
#include "PlayerExecuteAbility.generated.h"

class UAutobattlerControllerComponent;
class USceneComponent;

/**
 * Actor responsible for executing a player ability on the given targets.
 * Can be subclass to implement various visual effects.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API APlayerExecuteAbility : public AActor
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Default root component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIG
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* How long this actor is live for until it is destroyed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Player Ability")
	float DurationUntilSelfDestroy;

	/* For its lifetime, whether this ability sets its location to the location of its target. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler Player Ability")
	bool SnapsToTarget;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Whether this ability has been initialised. */
	bool HasBeenInitialised;

	/* Target actor to snap to if we want to follow it around. */
	UPROPERTY(Replicated);
	const AActor* SnapToActor;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	APlayerExecuteAbility();

	/**
	 * Used for actor snapping. 
	 */
	virtual void Tick(float DeltaSeconds) override;

/////////////////////////////////////////////////////////////////////////////////
//// NETWORKING
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Network scaffolding.
	 * @param OutLifetimeProps Unused.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

/////////////////////////////////////////////////////////////////////////////////
//// C++ API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Performs the executing of this ability on relevant target.
	 * @param PlayerAbilityData The ability which to get skill implementation from.
	 * @param SkillOwner Component which executed this ability.
	 * @param Target Target of this ability.
	 */
	void ExecuteAbility(FAutobattlerPlayerAbilityData* PlayerAbilityData, UAutobattlerControllerComponent* SkillOwner, AAutobattlerCharacter* Target);

/////////////////////////////////////////////////////////////////////////////////
//// BLUEPRINT API
/////////////////////////////////////////////////////////////////////////////////
protected:
	/**
	 * Called when the ability is executed. Can be used for visual effects.
	 * @param WhoOwns Owner of the skill.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Player Ability")
	void OnAbilityExecuted(UAutobattlerControllerComponent* SkillOwner, AAutobattlerCharacter* Target);
	virtual void OnAbilityExecuted_Implementation(UAutobattlerControllerComponent* SkillOwner, AAutobattlerCharacter* Target);
};
