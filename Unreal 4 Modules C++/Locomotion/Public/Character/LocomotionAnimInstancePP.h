// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enumerations/MovementType.h"
#include "Enumerations/Stance.h"
#include "LocomotionAnimInstancePP.generated.h"

/* The post process animation instance intended to be used by
ALocomotionCharacter. */
UCLASS()
class LOCOMOTION_API ULocomotionAnimInstancePP : public UAnimInstance
{
	GENERATED_BODY()
/* VARIABLES */
protected:
	/* ------------------- */
	/* CONFIGURABLE VALUES */
	/* ------------------- */

	UPROPERTY()
	class ALocomotionCharacter* CharacterReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoneNames")
	FName LeftFootBoneName = FName("foot_l");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoneNames")
	FName RightFootBoneName = FName("foot_r");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InterpolationSpeedSettings")
	float RotationOffsetInterpSpeed = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InterpolationSpeedSettings")
	float ZOffsetInterpSpeed = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSettings|TraceLengths")
	float TraceLengthAboveFoot = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSettings|TraceLengths")
	float TraceLengthBelowFoot = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSettings|TraceOffsetClamping")
	FVector MinOffsetStanding = FVector(-10.0f, -30.0f, -30.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSettings|TraceOffsetClamping")
	FVector MinOffsetCrouching = FVector(-10.0f, -30.0f, -15.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSettings|TraceOffsetClamping")
	FVector MaxOffsetStanding = FVector(10.0f, 30.0f, 20.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSettings|TraceOffsetClamping")
	FVector MaxOffsetCrouching = FVector(10.0f, 30.0f, 10.0f);

	/* --------------------- */
	/* VALUES FOR ANIM LOGIC */
	/* --------------------- */

	UPROPERTY(BlueprintReadOnly, Category = "ForPostProcessAnimGraph")
	FVector LeftFootOffset;

	UPROPERTY(BlueprintReadOnly, Category = "ForPostProcessAnimGraph")
	FVector RightFootOffset;

	UPROPERTY(BlueprintReadOnly, Category = "ForPostProcessAnimGraph")
	FTransform LeftFootTransform;

	UPROPERTY(BlueprintReadOnly, Category = "ForPostProcessAnimGraph")
	FTransform RightFootTransform;

	UPROPERTY(BlueprintReadOnly, Category = "ForPostProcessAnimGraph")
	float PelvisOffset;

	UPROPERTY(BlueprintReadOnly, Category = "ForPostProcessAnimGraph")
	EMovementType MovementType;

	UPROPERTY(BlueprintReadOnly, Category = "ForPostProcessAnimGraph")
	EStance Stance;

	UPROPERTY(BlueprintReadOnly, Category = "ForPostProcessAnimGraph")
	bool bEnableFootIK = true;

private:
	FVector LeftFootOffsetTargets;
	FVector RightFootOffsetTargets;

/* FUNCTIONS */
public:
	/* ------- */
	/* SETTERS */
	/* ------- */

	/* Set MovementType to NewMovementType. */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetMovementType(EMovementType NewMovementType);

	/* Set Stance to NewStance. */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetStance(EStance NewStance);

protected:
	/* -------- */
	/* REQUIRED */
	/* -------- */

	/* Called when post-process animation begins playing. Sets values from CharacterReference*. */
	UFUNCTION(BlueprintCallable, Category = "Main")
	void InitAnimationPostProcess();

	/* Called per frame. Updates leg IKs or ragdoll blends. */
	UFUNCTION(BlueprintCallable, Category = "Main")
	void UpdateAnimationPostProcess();

private:
	/* -------------------------------- */
	/* UTILITY FUNCTIONS FOR ANIM LOGIC */
	/* -------------------------------- */

	/* Update foot IK based on the feet's position to the ground. */
	void UpdateFootInverseKinematics();

	/* Update ragdoll IKs if Ragdolling. */
	void UpdateRagdollInverseKinematics();

	/* Clamp target foot offsets to Min/Max values, to prevent pelvis stretching. */
	FVector CalculatedClampedOffsets(float Roll, float Pitch, float Yaw);

	/* Interpolate the offset to the target. */
	FVector InterpolateOffset(FVector Current, FVector Target);
};
