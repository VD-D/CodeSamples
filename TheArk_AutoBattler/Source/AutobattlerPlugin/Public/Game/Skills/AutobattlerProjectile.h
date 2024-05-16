// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerProjectile.generated.h"

class AAutobattlerCharacter;
class UAutobattlerSkill;
class UDefaultAttack;
class UParticleSystem;
class UParticleSystemComponent;
class UProjectileMovementComponent;
class USceneComponent;
class UStaticMeshComponent;

/**
 * Handles projectile-type skills.
 */
UCLASS(Blueprintable)
class AUTOBATTLERPLUGIN_API AAutobattlerProjectile : public AActor
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ProjectileParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Whether or not we should check if we are within range of the target. */
	bool ShouldCheckDestination;

	/* How far away for the target to be considered "hit." */
	float MinDistanceToTarget;

	/* Who/where this projectile is targeting. */
	FAbilityTargetingProperties ProjectileTargetingProperties;

	/* Passed when the skill actually executes. */
	AAutobattlerCharacter* SkillOwnerInternal;

	/* Passed when the skill actually executes. */
	const UAutobattlerSkill* SkillImplementationInternal;

	/* Target of projectile movement. */
	USceneComponent* CurrentProjectileHomingTarget;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	AAutobattlerProjectile();

	/**
	 * Applies the effect as a projectile. The skill effect will occur when the projectile reaches the target character/location.
	 * @param SkillImplementation If initialising a skill, this is the skill implementation to use.
	 * @param SkillOwner Character who "owns" (executed) the skill
	 * @param TargetingProperties Properties which describe who/where is the target of the skill.
	 */
	void InitialiseProjectile(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties);

/////////////////////////////////////////////////////////////////////////////////
//// PROJECTILE
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Checks to see if projectile is within range of its target.
	 * @param DeltaTime Unused.
	 */
	virtual void Tick(float DeltaTime) override;

private:
	/**
	 * Updates visual components of this projectile for server and client.
	 * @param NewMesh Mesh to set.
	 * @param NewParticleSystem Particle system to set.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void UpdateVisuals(UStaticMesh* NewMesh, UParticleSystem* NewParticleSystem);

	/**
	 * Ensures both client and server have correct targeting properties for projectiles.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void UpdateProjectileTarget(USceneComponent* ProjectileHomingTarget);

	/**
	 * Called when the projectile reaches its destination.
	 */
	void OnArriveAtDestination();

/////////////////////////////////////////////////////////////////////////////////
//// DELEGATE BOUND
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Checks to see if character has died while projectile was travelling, and
	 * destroys this if that has happened.
	 * @param NewAction Tested to see if it is dead.
	 * @param UpdatedCharacter Character for whom action was updated for.
	 */
	UFUNCTION()
	void OnTargetCharacterStateChange(EActionType NewAction, AAutobattlerCharacter* UpdatedCharacter);
};
