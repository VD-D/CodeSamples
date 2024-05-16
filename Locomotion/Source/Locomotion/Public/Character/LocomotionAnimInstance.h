// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enumerations/ActiveLocomotionState.h"
#include "Enumerations/Gait.h"
#include "Enumerations/IdleEntryState.h"
#include "Enumerations/LocomotionActivity.h"
#include "Enumerations/MovementDirection.h"
#include "Enumerations/MovementType.h"
#include "Enumerations/RotationMode.h"
#include "Enumerations/Stance.h"
#include "Enumerations/WallClimbType.h"
#include "Structures/PivotParameters.h"
#include "Structures/WallClimbAssetData.h"
#include "LocomotionAnimInstance.generated.h"

/* The AnimInstance inteded to be used by ALocomotionCharacter. */
UCLASS()
class LOCOMOTION_API ULocomotionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

/* --------- */
/* VARIABLES */
/* --------- */
public:
	/* --- CONFIGURABLE ANIMATION MONTAGES --- */

	/* Montage for turning ninety degrees to the right. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Neutral Turn 90 Degrees")
	class UAnimMontage* NeutralTurnRightNinetyDegrees;

	/* Montage for turning ninety degrees to the left. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Neutral Turn 90 Degrees")
	class UAnimMontage* NeutralTurnLeftNinetyDegrees;

	/* Montage for turning 180 degrees to the right. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Neutral Turn 180 Degrees")
	class UAnimMontage* NeutralTurnRightHundredEightyDegrees;

	/* Montage for turning 180 degrees to the left. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Neutral Turn 180 Degrees")
	class UAnimMontage* NeutralTurnLeftHundredEightyDegrees;

	/* Montage for turning ninety degrees to the right while aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Aiming Turn 90 Degrees")
	class UAnimMontage* AimingTurnRightNinetyDegrees;

	/* Montage for turning ninety degrees to the left while aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Aiming Turn 90 Degrees")
	class UAnimMontage* AimingTurnLeftNinetyDegrees;

	/* Montage for turning ninety degrees to the left while crouching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Crouching Turn 90 Degrees")
	class UAnimMontage* CrouchingTurnRightNinetyDegrees;

	/* Montage for turning ninety degrees to the left while crouching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Crouching Turn 90 Degrees")
	class UAnimMontage* CrouchingTurnLeftNinetyDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Wall Climb Montages|1-Handed")
	FWallClimbAssetData WallClimbOneHandedDefault;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Wall Climb Montages|1-Handed")
	FWallClimbAssetData WallClimbOneHandedRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Wall Climb Montages|1-Handed")
	FWallClimbAssetData WallClimbOneHandedLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Wall Climb Montages|2-Handed")
	FWallClimbAssetData WallClimbTwoHandedDefault;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Animations|Wall Climb Montages|2-Handed")
	FWallClimbAssetData WallClimbTwoHandedCustom;

	/* The speed at which the character rotates about their Yaw. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Configurable Values|Axis Interpolate Speed")
	float AxisInterpolateSpeed = 10.0f;

	/* The maximal rotation which the character's head can turn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Configurable Values|Head Rotation")
	float MaxHeadRotation = 75.0f;

	/* The speed at which the character's head can turn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Configurable Values|Head Rotation")
	float HeadRotationInterpSpeed = 3.0f;

	/* The name of the curve which drives foot direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Configurable Values|Curve Names")
	FName FootDirectionCurveName = FName("FootDirection");

	/* The name of the curve which drives foot position. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocomotionCharacter|Configurable Values|Curve Names")
	FName FootPositionCurveName = FName("FootPosition");

	/* --- CURVES --- */

	/* The blend-in blend-out curve for flailing while falling. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curves")
	class UCurveFloat* FlailAlphaCurve;

	/* The blend-in blend-out curve for predicting when to land. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curves")
	class UCurveFloat* LandAlphaCurve;

protected:
	/* --- ANIM LOGIC VARIABLES --- */

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|FromLocomotionCharacterReference")
	float AimYawDelta;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|FromLocomotionCharacterReference")
	float AimYawRate;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|FromLocomotionCharacterReference")
	float WalkingSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|FromLocomotionCharacterReference")
	float RunningSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|FromLocomotionCharacterReference")
	float SprintingSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|FromLocomotionCharacterReference")
	float CrouchingSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	FRotator HeadLookingRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	FVector2D AimOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	FVector2D LookingAtAimOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	FVector2D LeanGrounded;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	FVector2D FeetPosition;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	EGait Gait;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	EMovementType MovementType;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	EMovementType MovementTypePrevious;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	EStance Stance;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	ERotationMode RotationMode;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	EMovementDirection MovementDirection;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	EIdleEntryState IdleEntryState;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	int32 UpperBodyLayeringIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	int32 FullBodyLayeringIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float GaitMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float CurrentSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float TargetCharacterRotationDifference;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float MovementInputVelocityDifference;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float NPlayRate;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float CPlayRate;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float FlailBlendAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float LeanInAir;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float LandPredictionAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float StartPosition;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	float FlailRate;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	bool bIsMoving;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	bool bHasMovementInput;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|UsefulForAnimGraph")
	bool bShouldUseHeadRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|TurnInPlace")
	bool bShouldTurnInPlace;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|TurnInPlace")
	bool bTurningInPlace;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|TurnInPlace")
	bool bTurningRight;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|TurnInPlace")
	float TurnInPlaceDelayCount;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|TurnInPlace")
	class UAnimMontage* ActiveTurnInPlaceMontage;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|LeaningCalculations")
	float PreviousSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|LeaningCalculations")
	float AccelerationDifference;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|LeaningCalculations")
	float VelocityDifference;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|LeaningCalculations")
	FRotator PreviousVelocityRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|AdditionalRotators")
	FRotator CharacterRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|AdditionalRotators")
	FRotator LastVelocityRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|AdditionalRotators")
	FRotator LastMovementInputRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|AdditionalRotators")
	FRotator LookingRotation;
	
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|ActiveLocomotionState")
	EActiveLocomotionState ActiveLocomotionState;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|PivotParameters")
	FPivotParameters PivotParams;


private:
	/* --- PRIVATE STATE VALUES --- */

	/* Location which is to be looked at by this character. Only relevant if bShouldUseHeadRotation = true. */
	FVector HeadLookAtLocation;

	/* When the TargetedActor is set, this is true. Otherwise it is false. */
	bool bIsTargeting;

	/* Current Leaning Rotation. */
	float LeanRotation;

	/* Current Leaning Acceleration. */
	float LeanAcceleration;

/* FUNCTIONS */
public:
	/* --- SETTERS --- */

	/** Sets ActiveLocomotionState to NewActiveLocomotionState.
	  * @param - NewActiveLocomotionState - EActiveLocomotionState to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetActiveLocomotionState(EActiveLocomotionState NewActiveLocomotionState);

	/** Sets RotationMode to NewRotationMode. 
	  * @param NewRotationMode - ERotationMode to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetRotationMode(ERotationMode NewRotationMode);

	/** Sets bIsAiming to NewAiming. 
	 * @param NewAiming - bool bIsAiming value to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetAiming(bool NewAiming);

	/** Sets Gait to NewGait. 
	  * @param NewGait - EGait to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetGait(EGait NewGait);

	/** Sets MovementType to NewMovementType. 
	  * @param NewMovementType - EMovementType to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetMovementType(EMovementType NewMovementType);

	/** Sets MovementDirection to NewMovementDirection. 
	  * @param NewMovementDirection - EMovementDirection to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetMovementDirection(EMovementDirection NewMovementDirection);

	/** Sets Stance to NewStance. 
	  * @param NewStance - EStance to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetStance(EStance NewStance);

	/** Sets IdleEntryState to NewIdleEntryState. 
	  * @param NewIdleEntryState - EIdleEntryState to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetIdleEntryState(EIdleEntryState NewIdleEntryState);

	/** Sets PivotParameters to NewPivotParameters. 
	  * @param NewPivotParameters - FPivotParameters to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetPivotParameters(FPivotParameters NewPivotParameters);

	/** Sets UpperBodyLayeringIndex to NewUpperBodyLayeringIndex.
	  * @param NewUpperBodyLayeringIndex - UpperBodyLayeringIndex to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetUpperBodyLayeringIndex(int32 NewUpperBodyLayeringIndex);

	/** Sets UpperBodyLayeringIndex to NewUpperBodyLayeringIndex.
	 * @param NewUpperBodyLayeringIndex - UpperBodyLayeringIndex to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetFullBodyLayeringIndex(int32 NewFullBodyLayeringIndex);

	/** Sets bShouldUseHeadRotation to NewUseHeadRotation.
	  * @param NewUseHeadRotation - New bShouldUseHeadRotation to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetUseHeadRotation(bool NewUseHeadRotation);

	/** Sets HeadLookAtLocation to NewHeadLookAtLocation. Only relevant if bShouldUseHeadRotation = true. 
	  * @param NewHeadLookAtLocation - New HeadLookAtLocation to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetHeadLookAtLocation(FVector NewHeadLookAtLocation);

	/* --- ANIM MONTAGES --- */

	/** Returns the wall climb asset data for a given type of WallClimbType.
	  * @param WallClimbType - The EWallClimbType to return the asset data for.
	  * @return A FWallClimbAssetData struct containing relevant information.
	  */
	FWallClimbAssetData GetWallClimbDataFromType(EWallClimbType WallClimbType);

	/** This function is intended to be called from UTurningInPlaceNotifyState. It updates ActiveAnimMontage,
	  * bShouldTurnInPlace, bTurningInPlace and bTurningRight.
	  * @param TurnInPlaceMontage - The turn in place montage to set.
	  * @param ShouldTurnInPlace - Whether the character should turn in place or not.
	  * @param TurningInPlace - Whether the character is currently turning in place or not.
	  * @param TurningRight - If the character is turning right. By implication, if TurningRight = false, the character is turning left.
	  */
	UFUNCTION(BlueprintCallable, Category = "TurnInPlace")
	void NotifiedTurningInPlace(class UAnimMontage* TurnInPlaceMontage, bool ShouldTurnInPlace, bool TurningInPlace, bool TurningRight);

protected:
	/** Function which is meant to be called at the start of the animation. Due
	  * to vagaries of Unreal, this must be called by an instance which inherits
	  * from this anim instance, connected to the InitAnimation node.
	  * This function effectively replaces the constructor 
	  */
	UFUNCTION(BlueprintCallable, Category = "InitialiseAnimation")
	void InitAnimation();

	/** This is the per-frame called function which needs to be connnected to the
	  * BlueprintUpdateAnimation node. It effectively serves as a wrapper for other
	  * function which validate LocomotionCharacterReference and perform various operations
	  * related to animation logic. 
	  * @param DeltaTimeX - World Delta time.
	  */
	UFUNCTION(BlueprintCallable, Category = "UpdateAnimation")
	void UpdateAnimation(float DeltaTimeX);

	/* --- UTILITY --- */

	/** Function which calls PlaySlotAnimationAsDynamicMontage with the passed animation. Used to play idle transitions smoothly.
	  * @param AnimationToPlay - The idle transition animation to play.
	  * @param InPlayRate - The play rate to play AnimationToPlay at.
	  * @param InTimeStartMontage - The time to start the dynamic montage at.
	  */
	UFUNCTION(BlueprintCallable, Category = "IdleTransitions")
	void IdleTransition(class UAnimSequenceBase* AnimationToPlay, float InPlayRate, float InTimeToStartMontage);

	/** Calculates the positions of the feet. */
	UFUNCTION(BlueprintCallable, Category = "Utility")
	void CalculateFeetPosition();

	/** Plays a montage if that montage is valid. 
	  * @param Montage - The montage to play.
	  * @param SectionName - The section of the montage to jump to. Only works if IsValidSectionName(SectionName) = true.
	  */
	void PlayMontageIfValid(class UAnimMontage* Montage, FName SectionName);

private:
	/* --- PRIVATE FUNCTIONS USED ONLY FOR ANIMATION LOGIC --- */

	/** This updates AimOffset based on either the camera direction or the
	  * direction the character is moving in, depending on RotationMode.
	  */
	void CalculateLookingAimOffset();

	/** Calculates the value of the head rotation. Only relevant if bShouldUseHeadRotation = true.
	  */
	void CalculateHeadRotation();

	/** This calculates GaitMultiplier, which is a value between 0.0f to 3.0f,
	  * depending on whether the character is stationary, walking, running or sprinting.
	  * There is no hard cutoff, e.g. if the character is blending between standing still
	  * and walking, GaitMultiplier might be something like 0.5f.
	  */
	void CalculateGaitMultiplier();      

	/** This updates NPlayRate and CPlayRate. These values determine how fast the playback of animations is, and are calculated
	  * from CurrentSpeed and GaitMultiplier. It allows certain animations to be played faster or slower at certain gaits, so
	  * for example, turning is slower when running then when walking.
	  * @param WalkAnimSpeed - The character's walking speed.
	  * @param RunAnimSpeed - The character's running speed.
	  * @param SprintAnimSpeed - The character's sprinting speed.
	  * @param CrouchAnimSpeed - The character's crouched speed.
	  * @param CapsuleComponent - The character's capsule component.
	  */
	void CalculateAnimPlayRates(float WalkAnimSpeed, float RunAnimSpeed, float SprintAnimSpeed, float CrouchAnimSpeed, class UCapsuleComponent* CapsuleComponentReference);

	/** This sets MovementDirection to either Forward or Backward, depending on Direction. The buffer is how much tolerance there
	  * is over the threshold, mostly so that MovementDirection does not flip-flop when Direction is right on the threshold.
	  * @param DirectionThresholdMin - The length of Direction must be above this value - Buffer.
	  * @param DirectionThresholdMax - The length of Direction must be below this value + Buffer.
	  * @param Buffer - Added tolerance to the direction threshold range.
	  */
	void CalculateMovementDirection(float DirectionThresholdMin, float DirectionThresholdMax, float Buffer);

	/** Instantly plays a turn right or turn left anim montage, assuming they are not already turning. This function will 
	  * also interrupt and play  the opposite anim montage if the character turns around suddenly. 
	  * @param AimYawLimit - The threshold which must be exceeded for the character to be considered to be turning around suddenly.
	  * @param TurnLeftMontage - The montage to play when turning left.
	  * @param TurnRightMontage - The montage to play when turning right.
	  * @param PlayRate - The play rate of the turning montages.
	  */
	void TurnInPlaceResponsive(float AimYawLimit, class UAnimMontage* TurnLeftMontage, class UAnimMontage* TurnRightMontage, float PlayRate);

	/** A more complicated TurnInPlace function, this increases a turn in place delay counter (which is scaled 
	  * between AimYawLimit1 and AimYawLimit2) and only plays a turn in place anim montage. This is useful (for example) 
	  * when determining whether or not the player should turn in place 90 degrees or 180 degrees. 
	  * @param MaxCameraSpeed - The camera (or controller rotation) speed which must be exceeded to turn 180 degrees.
	  * @param AimYawLimit1 - The AimYawLimit of turning 90 degree montages.
	  * @param AimYawLimit2 - The AimYawLimit of turning 180 degree montages.
	  * @param PlayRate1 - The play rate of the 90 degree turning montages.
	  * @param PlayRate2 - The play rate of the 180 degree turning montages.
	  * @param DelayTime1 - How much time needs to expire before playing turning 90 degrees montages.
	  * @param DelayTime2 - How much time needs to expire before playing turning 180 degrees montages.
	  * @param TurnLeftMontage1 - The montage to play when turning left 90 degrees.
	  * @param TurnRightMontage1 - The montage to play when turning right 90 degrees.
	  * @param TurnLeftMontage2 - The montage to play when turning left 180 degrees.
	  * @param TurnRightMontage2 - The montage to play when turning right 180 degrees.
	  */
	void TurnInPlaceDelayed(float MaxCameraSpeed, float AimYawLimit1, float DelayTime1, float PlayRate1, class UAnimMontage* TurnLeftMontage1, class UAnimMontage* TurnRightMontage1, float AimYawLimit2, float DelayTime2, float PlayRate2, class UAnimMontage* TurnLeftMontage2, class UAnimMontage* TurnRightMontage2);
};
