// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Types/AutobattlerEnums.h"
#include "AutobattlerAnimInstance.generated.h"

class AAutobattlerCharacter;
class UAnimationAsset;

/**
 * Animation instance to be used by autobattler characters.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// ANIMATION VARIABLES
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* This character's movement speed (velocity) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Autobattler Animations")
	float MovementSpeed;

	/* How fast the playback of the skill animation should be. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Autobattler Animations")
	float SkillActionSpeed;

	/* How fast the playback of the skill animation should be. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Autobattler Animations")
	float AttackActionSpeed;

	/* Action type driving animation states. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Autobattler Animations")
	EActionType ActionType;

	/* Action type driving animation states. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Autobattler Animations")
	UAnimationAsset* SkillAnimation;

	/* Action type driving animation states. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Autobattler Animations")
	UAnimationAsset* AttackAnimation;

	/* Animations to use when performing a skill. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Animations")
	TArray<UAnimationAsset*> SkillAnimations;

	/* Animations to use when attacking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Animations")
	TArray<UAnimationAsset*> AttackAnimations;

	/* Animations to use when dying. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Animations")
	TArray<UAnimationAsset*> DeathAnimations;

	/* Animations to use when victorious. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler Animations")
	TArray<UAnimationAsset*> VictoryAnimations;

/////////////////////////////////////////////////////////////////////////////////
//// ANIMATION API
/////////////////////////////////////////////////////////////////////////////////
protected:
	/**
	 * Native update (tick) used to get MovementSpeed from the pawn owner.
	 * @param DeltaSeconds Unused.
	 */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

/////////////////////////////////////////////////////////////////////////////////
//// EXTERNAL SETTERS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/**
	 * Sets the action type. Should only be called by the owning Autobattler Character.
	 * @param NewActionType Action Type to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Anim Instance")
	void SetActionType(EActionType NewActionType);

	/**
	 * Resets the AI State. Should be called after attack or skill animation conclude.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Anim Instance")
	void ResetAIState();

private:
	/**
	 * Sets the skill animation and its speed.
	 * @param AttackAnimation The skill animation itself.
	 * @param AttackSpeed How fast the skill animation should play.
	 */
	void SetSkillParams(UAnimationAsset* NewSkillAnimation, float NewSkillSpeed);

	/**
	 * Sets the attack animation and its speed.
	 * @param AttackAnimation The attack animation itself.
	 * @param AttackSpeed How fast the attack animation should play.
	 */
	void SetAttackParams(UAnimationAsset* NewAttackAnimation, float NewAttackSpeed);

	/* Marked as friend, so that it can change the action type. */
	friend class AAutobattlerCharacter;
};
