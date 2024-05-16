// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/AutobattlerStructs.h"
#include "ExecuteSkill.generated.h"

class AAutobattlerCharacter;
class UAutobattlerSkill;
class UDefaultAttack;
class UParticleSystemComponent;
class USceneComponent;
class USkillImplementation;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FSkillImplementationContainer
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkillImplementation* ImplementationObject;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AAutobattlerCharacter* SkillOwner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESkillTargetingMode SkillTargetingMode; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AAutobattlerCharacter* Target; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector TargetLocation;

	FSkillImplementationContainer()
	{
		ImplementationObject = nullptr;
		SkillOwner = nullptr;
		SkillTargetingMode = ESkillTargetingMode::None;
		Target = nullptr;
		TargetLocation = FVector::ZeroVector;
	}

	FSkillImplementationContainer(
		USkillImplementation* NewImplementationObject, 
		AAutobattlerCharacter* NewSkillOwner, 
		ESkillTargetingMode NewSkillTargetingMode,
		AAutobattlerCharacter* NewTarget,
		const FVector& NewTargetLocation
	)
	{
		ImplementationObject = NewImplementationObject;
		SkillOwner = NewSkillOwner;
		SkillTargetingMode = NewSkillTargetingMode;
		Target = NewTarget;
		TargetLocation = NewTargetLocation;
	}
};

/**
 * Actor which is responsible for a skill trigger. It can take the form of a projectile or effect if
 * the effect is one of these. 
 */
UCLASS()
class AUTOBATTLERPLUGIN_API AExecuteSkill : public AActor
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Root of this actor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* EffectMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* EffectParticle;

/////////////////////////////////////////////////////////////////////////////////
//// PRIVATE & PROTECTED VARIABLES
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Have we already initialised this skill. */
	bool HasBeenInitialised;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Default constructor.
	 */
	AExecuteSkill();

	/**
	 * Applies the effect instantly.
	 * @param SkillImplementation If initialising a skill, this is the skill implementation to use.
	 * @param SkillOwner Character who "owns" (executed) the skill
	 * @param TargetingProperties Properties which describe who/where is the target of the skill.
	 * @param DestroyOnEnd Whether this actor should be destroyed after the effect fires.
	 */
	virtual void InitialiseAsInstant(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties, bool DestroyOnEnd = true);

	/**
	 * Creates a projectile to apply the effect. The skill effect will occur when the projectile reaches the target character/location.
	 * @param SkillImplementation If initialising a skill, this is the skill implementation to use.
	 * @param SkillOwner Character who "owns" (executed) the skill
	 * @param TargetingProperties Properties which describe who/where is the target of the skill.
	 */
	virtual void InitialiseAsProjectile(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties);

	/**
	 * Similar to InitialiseAsInstant, except that it will also spawn a projectile and/or mesh at the target character/location.
	 * @param SkillImplementation If initialising a skill, this is the skill implementation to use.
	 * @param SkillOwner Character who "owns" (executed) the skill
	 * @param TargetingProperties Properties which describe who/where is the target of the skill.
	 */
	virtual void InitialiseAsEffect(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties);

private:
	/**
	 * Internal function for executing all given skill effects of a skill.
	 * @param SkillImplementation If initialising a skill, this is the skill implementation to use.
	 * @param SkillOwner Character who "owns" (executed) the skill
	 * @param TargetingProperties Properties which describe who/where is the target of the skill.
	 */
	void ExecuteSkillList(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties);

	/**
	 * Updates skill visual components. Only relevant if skill type is effect.
	 * @param NewMesh Mesh representation of skill.
	 * @param NewTemplate Effect particle template.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void UpdateEffectVisuals(UStaticMesh* NewMesh, UParticleSystem* NewTemplate);

	/**
	 * Relevant only if skill type is effect. Destroys the skill after a cooldown.
	 */
	UFUNCTION()
	void OnEffectEnd();
};
