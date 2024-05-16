// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Enumerations/CardinalDirection.h"
#include "Enumerations/Gait.h"
#include "Enumerations/MovementType.h"
#include "Enumerations/RotationMode.h"
#include "Enumerations/Stance.h"
#include "Enumerations/WallClimbType.h"
#include "Enumerations/WeaponType.h"
#include "Structures/ClimbableLedgeData.h"
#include "Structures/WallClimbAssetData.h"
#include "Structures/WallClimbParameters.h"
#include "Structures/WallClimbTraceSettings.h"
#include "LocomotionCharacter.generated.h"

/* A generic character which has moving, jumping and wall mantling capabilities, as well 
as capabilities for equipping weapons. Does not implement SetupPlayerInputComponent and 
does not have a camera; these should be added by a child class. */
UCLASS()
class LOCOMOTION_API ALocomotionCharacter : public ACharacter
{
	GENERATED_BODY()

/* --------- */
/* VARIABLES */
/* --------- */
public:
	/* --- CONFIGURABLE MOVEMENT VALUES --- */

	/* The character's walking speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float WalkingSpeed;

	/* The character's running speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float RunningSpeed;

	/* The character's sprinting speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float SprintingSpeed;

	/* The character's movement speed when crouched. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float CrouchingSpeed;

	/* The character's acceleration when walking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float WalkingAcceleration;

	/* The character's acceleration when running or sprinting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float RunningAcceleration;

	/* The character's deceleration when walking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float WalkingDeceleration;

	/* The character's deceleration when running or sprinting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float RunningDeceleration;

	/* Ground friction when walking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float WalkingFriction;

	/* Ground friction when running or sprinting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Movement Settings")
	float RunningFriction;

	/* --- CONFIGURABLE WALL CLIMBING/MANTLING VALUES --- */

	/* The collision channel which is used to determine what is climable or not. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Wall Climbing Configuration|Traces")
	TEnumAsByte<ECollisionChannel> WallClimbCollisionChannel;

	/* The trace setting for wall climbing/mantling when grounded. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Wall Climbing Configuration|Traces")
	FWallClimbTraceSettings GroundedWallClimbTraceSettings;

	/* The trace setting for wall climbing/mantling when falling. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Wall Climbing Configuration|Traces")
	FWallClimbTraceSettings FallingWallClimbTraceSettings;

	/* The interpolation curve for low wall climbs. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Wall Climbing Configuration|Curves")
	class UCurveFloat* WallClimbTimelineCurveLow;

	/* The interpolation curve for high wall climbs. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Wall Climbing Configuration|Curves")
	class UCurveFloat* WallClimbTimelineCurveHigh;

	/* --- BONE NAME CONFIGURATION --- */

	/* Name of pelvis or hip bone. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Bone Names")
	FName PelvisBone;

	/* Name of bone to apply impulse to when ragdolling. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Bone Names")
	FName ImpulseBoneName;

	/* --- RAGDOLL CONFIGURATION --- */

	/* The power of the impulse applied to ImpulseBoneName. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion Character|Ragdoll")
	float ImpulseModifier;

protected:
	/* --- LOCOMOTION STATE VALUES --- */

	/* Whether or not the character is moving. */
	bool bIsMoving;

	/* Whether or not the character has movement input.
	This is different from bIsMoving, since a character may not have movement input but still be
	moving (e.g. decelerating). */
	bool bHasMovementInput;

	/* Whether the character should try to sprint. */
	bool bShouldSprint;

	/* Direction of the movement input. */
	FVector MovementInput;

	/* The character mesh's current rotation. This is NOT the actor's rotation. */
	FRotator CharacterRotation;

	/* The rotation the character is looking in. Used to adjust the head. */
	FRotator LookingRotation;

	/* The rotation of the last velocity-direction vector. */
	FRotator LastVelocityRotation;

	/* The rotation of how the last input rotated the character. */
	FRotator LastMovementInputRotation;

	/* Used for normalising AimYawRate. */
	float AimYawDelta;

	/* The rate at which the character can rotate their Yaw (Z) axis. */
	float AimYawRate;

	/* The difference between the movement input and the current velocity. */
	float MovementInputVelocityDifference;

	/* The difference between the target rotation and the current rotation. */
	float TargetCharacterRotationDifference;

	/* The direction the character is current going in (an angle between 0 and 360). */
	float Direction;

	/* The value of the forward axis (usually when moving forward or backward). */
	float ForwardAxisValue;

	/* The value of the right axis (usually when moving right or left). */
	float RightAxisValue;

	/* Whether the character is rotated north, south, east or west. */
	ECardinalDirection CardinalDirection;

	/* This character's allowed stances. By default, all are allowed. */
	TArray<EStance> AllowedStances;

	/* This character's allowed gaits. By default, all are allowed. */
	TArray<EGait> AllowedGaits;

	/* The rotation the character wants to rotate to when walking. */
	FRotator TargetRotation;

	/* The rotation the character wants to rotate to when jumping or falling.*/
	FRotator JumpRotation;

	/* The current offset between the character's cardinal direction and their current rotation. */
	float RotationOffset;

	/* The character's rotation rotate toward their rotation offset. */
	float RotationMultiplier;

	/* --- WALL CLIMBING TIMELINE --- */

	/* Timeline, called by WallClimbBegin(). */
	UPROPERTY()
	class UTimelineComponent* WallClimbTimeline;

	/* The curve used by WallClimbTimeline. */
	UPROPERTY()
	class UCurveFloat* WallWalkCurve;

	/* --- RAGDOLL VALUES --- */

	/* If ragdolling, is the ragdoll on the ground. */
	bool bRagdollOnGround;

	/* If ragdolling, what is the ragdoll's location in relation to the actor's location. */
	FVector RagdollLocation;

	/* If ragdolling, what is the ragdoll's rotation in relation to the actor's rotation. */
	FVector RagdollVelocity;

private:
	/* --- PRIVATE STATE VALUES --- */

	/* The character's current gait (Walking, Running or Sprinting). */
	EGait Gait;

	/* The character's current stance (Standing or Crouching). */
	EStance Stance;

	/* Whether the character is looking in the direction of their velocity, or in the direction of a point in space. */
	ERotationMode RotationMode;

	/* Whether the character is grounded, falling, ragdolling or wall walking. */
	EMovementType MovementType;

	/* Whether the character was previously grounded, falling, ragdolling or wall walking. */
	EMovementType MovementTypePrev;

	/* Whether or not the character is aiming. */
	bool bIsAiming;

	/* The index of upper-body layering. This allows for the linking of anim graphs to play on a locomotion character. */
	int32 UpperBodyLayeringIndex;

	/* The index of full-body layering. This allows for the linking of anim graphs to play on a locomotion character. */
	int32 FullBodyLayeringIndex;

	/* --- WALL CLIMBING RELATED --- */

	/* The transform when climbing up on a wall. */
	FTransform WallTargetTransform;

	/* The actual offset transform when climbing up on a wall. */
	FTransform WallClimbActualStartOffset;

	/* The offset transform for the animation when climbing up on a wall. */
	FTransform WallClimbAnimStartOffset;

	/* The current parameters for wall climbing. */
	FWallClimbParameters LocalWallClimbParameters;

	/* The current climbable ledge data converted to local space. */
	FClimbableLedgeData ClimbableLedgeDataLocalSpace;

	/* INTERNAL USE ONLY. Binds the wall climbing callback to the timeline. */
	UPROPERTY()
	FOnTimelineFloat OnWallClimbingTimelineCallback;

	/* --- TIMER RELATED --- */

	/* Handle for calling OnLandedTimerHandleEnd */
	FTimerHandle LandedTimerHandle;

	/* --- SPAWNING RELATED --- */

	/* Static Mesh Components spawned by SetProp(). */
	TArray<class UStaticMeshComponent*> SpawnedStaticMeshComps;

	/* Skeletal Mesh Components spawned by SetProp(). */
	TArray<class USkeletalMeshComponent*> SpawnedSkeletalMeshComps;

	/* Whether the last spawned component from SetProp() was a skeletal or static mesh component. */
	bool bWasLastSpawnedCompStaticMesh;

	/* --- UTILITY --- */

	/* Actors to ignore for traces. Defaults to 'this'. */
	TArray<AActor*> TraceActorsToIgnore;

	bool bGateOneOpen;
	bool bGateTwoOpen;

/* --- FUNCTIONS --- */
public:
	/* --- REQUIRED --- */

	/** Constructor */
	ALocomotionCharacter();

	/** Updates movement values every frame, depending on MovementType. */
	virtual void Tick(float DeltaTime) override;

	/* --- SETTERS --- */

	/** Changes RotationMode to NewRotationMode (either velocity or target direction). 
	  * @param NewRotationMode - The ERotationMode to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Setters", meta = (DisplayName = "Set Rotation Mode"))
	void SetRotationMode(ERotationMode NewRotationMode);

	/** Sets bIsAiming to NewAiming and updates character aiming. 
	  * @param NewAiming - The bIsAiming to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Setters", meta = (DisplayName = "Set Is Aiming"))
	void SetIsAiming(bool NewAiming);

	/** Sets Gait to NewGait.
	  * @param NewGait - The EGait to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Setters", meta = (DisplayName = "Set Gait"))
	void SetGait(EGait NewGait);

	/** Sets MovementType to NewMovementType and updates character movement. 
	  * @param NewMovementType - The EMovementType to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Setters", meta = (DisplayName = "Set Movement Type"))
	void SetMovementType(EMovementType NewMovementType);

	/** Set Stance to NewStance and updates character movement. 
	  * @param NewStance - The EStance to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Setters", meta = (DisplayName = "Set Stance"))
	void SetStance(EStance NewStance);

	/** Sets UpperBodyLayeringIndex to NewUpperBodyLayeringIndex.
	  * @param NewUpperBodyLayeringIndex - The new upper body layering index to set. Clamped to a minimum of 0.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Setters", meta = (DisplayName = "Set Upper Body Layering Index"))
	void SetUpperBodyLayeringIndex(int32 NewUpperBodyLayeringIndex);

	/** Sets FullBodyLayeringIndex to NewFullBodyLayeringIndex.
	  * @param NewFullBodyLayeringIndex - The new upper body layering index to set. Clamped to a minimum of 0.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Setters", meta = (DisplayName = "Set Upper Body Layering Index"))
	void SetFullBodyLayeringIndex(int32 NewFullBodyLayeringIndex);

	/** Sets allowed Gaits and Stances on this character. 
	  * Forces the character to switch to the first gait in NewAllowedGaits and the first stance in NewAllowedStances if their
	  * current Gait/Stance is not contained within the array.
	  * @param NewAllowedGaits - Array of allowed gaits. Does not set anything if NewAllowedGaits is empty.
	  * @param NewAllowedStances - Array of allowed stances. Does not set anything if NewAllowedStances is empty.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Setters", meta = (DisplayName = "Set Allowed Movement Models"))
	virtual void SetAllowedMovementModels(const TArray<EGait> NewAllowedGaits, const TArray<EStance> NewAllowedStances);

	/** Spawns a static mesh or skeletal mesh on this character's skeletal mesh at a named socket. Will prefer to spawn the skeletal mesh
	  * over the static mesh if both are not null.
	  * @param SocketName - Name of the socket on the character's mesh. Must be valid, otherwise nothing happens. Not used if ClearOnly = true.
	  * @param ClearAll - If true, will attempt to remove all static and skeletal meshes attached by this function.
	  * @param Clear - If true, will attempt to remove the last attached static or skeletal mesh. Not relevant if ClearAll = true.
	  * @param StaticMesh - The static mesh to attach.
	  * @param SkeletalMesh - The skeletal mesh to attach.
	  * @param SpawnedStaticMeshComp - The static mesh component spawned. WARNING: This will be null if not spawning a static mesh or if something goes wrong.
	  * @param SpawnedSkeletalMeshComp - The skeletal mesh component spawned. WARNING: This will be null if not spawning a skeletal mesh or if something goes wrong.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Spawning", meta = (DisplayName = "Spawn Prop"))
	void SetProp(class UStaticMeshComponent*& NewlySpawnedStaticMeshComp, class USkeletalMeshComponent*& NewlySpawnedSkeletalMeshComp, FName SocketName, bool ClearAll = false, bool Clear = true, class UStaticMesh* StaticMesh = nullptr, class USkeletalMesh* SkeletalMesh = nullptr);

	/* --- GETTERS --- */

	/** Gets all relevant movement properties from this character. All parameters are mutable.
	  * @param CurrentVelocity - Pawn's current velocity.
	  * @param CurrentCharacterRotation - Character's current rotation w.r.t. their desired rotation.
	  * @param CurrentLastVelocityRotation - Last known rotation w.r.t. the character's velocity rotation.
	  * @param CurrentLastMovementInputRotation - Last known rotation w.r.t. to input on the controller.
	  * @param CurrentLookingRotation - The rotation the character is currently looking in.
	  * @param CurrentDirection - Character's current directional facing, i.e. a value between 0 to 360 (in world space).
	  * @param TargetCharacterRotationDifference - Absolute value of CurrentLookingRotation.Yaw - CharacterRotaiton.Yaw.
	  * @param CurrentMovementInputVelocityDifference - Absolute value of CurrentLastVelocityRotation.Yaw - CurrentLastMovementInputRotation.Yaw.
	  * @param CurrentAimYawDelta - Normalized aim yaw.
	  * @param CurrentAimYawRate - How fast aim yaw is changing currently.
	  * @param bIsCurrentlyMoving - Whether or not the character is currently moving.
	  * @param bCurrentHasMovementInput - Whether or not the character has movement input.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Getters", meta = (DisplayName = "Get Movement Properties"))
	void GetMovementProperties(FVector& CurrentVelocity, FRotator& CurrentCharacterRotation, FRotator& CurrentLastVelocityRotation, FRotator& CurrentLastMovementInputRotation,
		FRotator& CurrentLookingRotation, float& CurrentDirection, float& CurrentTargetCharacterRotationDifference, float& CurrentMovementInputVelocityDifference,
		float& CurrentAimYawDelta, float& CurrentAimYawRate, bool& bIsCurrentMoving, bool& bCurrentHasMovementInput);

	/** Gets all relevant movement states for this character. All parameters are mutable.
	  * @param CurrentRotationMode - Character's current rotation mode.
	  * @param CurrentGait - Character's current gait.
	  * @parma CurrentMovementType - Character's current movement type.
	  * @param PreviousMovementType - Character's previous movement type.
	  * @param CurrentStance - Character's current stance.
	  * @param bIsCurrentlyAiming - Whether or not the character is currently aiming.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Getters", meta = (DisplayName = "Get Movement States"))
	void GetMovementStates(ERotationMode& CurrentRotationMode, EGait& CurrentGait, EMovementType& CurrentMovementType, EMovementType& PreviousMovementType, EStance& CurrentStance, bool& bIsCurrentlyAiming);

	/** Gets current upper body and full body layering indicies. All parameters are mutable.
	  * @param CurrentUpperBodyLayeringIndex - Character's current upper body layering index.
	  * @param CurrentFullBodyLayeringIndex - Character's current full body layering index.
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Getters", meta = (DisplayName = "Get Layering Indicies"))
	void GetLayeringIndicies(int32& CurrentUpperBodyLayeringIndex, int32& CurrentFullBodyLayeringIndex);

	/* --- WALL CLIMBING/MANTLING SYSTEM --- */

	/** When executed, the character will test to see if they can climb or vault over a wall. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Climb"))
	void WantsToClimb();

	/* --- UTILITY --- */

	/** Returns character velocity or physics linear velocity if ragdolling. 
	  * @return Velocity of the character or the ragdoll.
	  */
	FVector ChooseVelocity();

	/** Adds rotation to the character by the specified amount. 
	  * @param AddAmount - The amount to add.
	  */
	void AddCharacterRotation(FRotator AddAmount);

	/** Specifies desired looking rotation. By default returns GetControlRotation().
	  * @return Desired looking rotation to rotate toward.
	  */
	UFUNCTION(BlueprintNativeEvent, Category = "Locomotion Character|Setters", meta = (DisplayName = "Set Desired Looking Rotation"))
	FRotator SetLookingRotation();

	virtual FRotator SetLookingRotation_Implementation();

protected:
	/** Called when the game starts or when spawned. Sets certain default values. */
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	/* PERFORM MOVEMENT ACTIONS */

	/* ====================== THIS SECTION NEEDS TO BE MODIFIED ====================== */
	/* Only calls super. */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/* ====================== THIS SECTION NEEDS TO BE MODIFIED ====================== */

	/** Forward or backward movement.
	  * @param Value - Axis value which scales velocity (usually 1.0f for forward, -1.0f for backward).
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Movement", meta = (DisplayName = "Move Forward"))
	void MoveForward(float Value);

	/** Right or left movement. 
	  * @param Value - Axis value which scales velocity (usually 1.0f for right, -1.0f for left).
	  */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Movement", meta = (DisplayName = "Move Right"))
	void MoveRight(float Value);

	/** Called when jump action is pressed. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Jump Pressed"))
	void JumpActionPressed();

	/** Called when jump action is released. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Jump Released"))
	void JumpActionReleased();

	/** Called to change stance (stand to crouch of vice versa). */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Change Stance"))
	void StanceAction();

	/** Called when stance is changed. */
	void OnStanceChange(EStance EnteredStance);

	/** Called when switching to walk. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Walk"))
	void WalkAction();

	/** Called when sprinting action is pressed. If walking, run instead. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Sprint Pressed"))
	void SprintActionPressed();

	/** Called when sprinting action is released. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Sprint Released"))
	void SprintActionReleased();

	/** Called when character wishes to start using aiming rotation. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Aim Pressed"))
	void AimActionStart();

	/** Called when character wishes to stop using aiming rotation. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Aim Released"))
	void AimActionStop();

	/** Changes rotation mode from velocity to camera or camera to velocity. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion Character|Actions", meta = (DisplayName = "Change Rotation Mode"))
	void NewRotationMode();

	/* --- CHARACTER MOVEMENT COMPONENT OVERRIDES --- */

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void Landed(const FHitResult& Hit) override;

	/* --- RAGDOLL FUNCTIONS --- */

	/** Function which ragdolls the character.
	  * @param HitDirection - Optional direction which will apply impulse when ragdolling.
	  */
	void ToRagdoll(FVector HitDirection);

private:
	/* --- UTILITY --- */

	/** Wrapper for calling all 4 Choose-type functions. */
	void UpdateCharacterMovement();

	/** Selects max walking speed. 
	  * @return The character's walking speed based on stance, gait and aiming (returns max walk speed).
	  */
	float ChooseMaxWalkSpeed();

	/** Selects friction.
	  * @return Friction based on whether character is walking or running (returns friction). 
	  */
	float ChooseGroundFriction();

	/** Selects acceleration.
	  * @return Acceleration based on gait (returns max acceleration). 
	  */
	float ChooseMaxAcceleration();

	/** Selects deceleration.
	  * @return Deceleration based on gait (returns deceleration.) 
	  */
	float ChooseDeceleration();

	/** Gets the location of where the character's capsule touches the floor. 
	  * @param ZOffset - How much the final value should be moved up/down on the Z axis.
	  * @return The location where the character's capsule touches the floor. 
	  */
	FVector GetCapsuleFloorLocation(float ZOffset);

	/** Subtracts one transform from the other, component wise. In other words, subtracts A.Translation - B.Translation,
	  * A.Rotation.Roll - B.Rotation.Roll, A.Rotation.Yaw - B.Rotation.Yaw, A.Rotation.Pitch - B.Rotation.Pitch and
	  * A.Scale - B.Scale.
	  * @param A - The first transform.
	  * @param B - The second transform.
	  * @return A FTransform subtracted component wise from A to B.
	  * @warning This operation is not commutative. SubTransformByComponents(A, B) != SubTransformByComponents(B, A).
	  */
	FTransform SubTransformByComponents(FTransform A, FTransform B);

	/** Add one transfrom to the other, component wise. In other words, adds A.Translation + B.Translation,
	  * A.Rotation.Roll + B.Rotation.Roll, A.Rotation.Roll + B.Rotation.Roll, A.Rotation.Pitch + B.Rotation.Pitch and
	  * A.Scale + B.Scale.
	  * @param A - The first transfrom.
	  * @param B - The second transform.
	  * @return A FTransfrom added component wise from A to B.
	  */
	FTransform AddTransformByComponents(FTransform A, FTransform B);

	/* --- WALL CLIMBING/MANTLING UTILITY --- */

	/** Performs tests to see if the character can or cannot wall climb or mantle. 
	  * @param WallClimbTraceSettings - A FWallClimbTraceSettings which determines how traces are handled.
	  * @return Whether or not the character can climb/mantle or not.
	  */
	bool WallClimbCheck(FWallClimbTraceSettings WallClimbTraceSettings);

	/** Begins wall climbing/mantling (called internally from WallClimbCheck).
	  * @param WallClimbHeight - The difference of the position between the character's current capsule location and their target capsule location.
	  * @param ClimbableLedgeData - FClimableLedgeData struct which stores the targeted transform and the hit climable actor.
	  * @param WallClimbType - Whether the WallClimbType is High, Low or Fall.
	  */
	void WallClimbBegin(float WallClimbHeight, FClimbableLedgeData ClimbableLedgeData, EWallClimbType WallClimbType);

	/** Updates the character's position when wall walking. Is intended to be executed by WallClimbTimelineCallback(). 
	  * @param Blend - The current time on the timeline, corresponding to how much of the climbing animation has blended in.
	  */
	UFUNCTION()
	void WallClimbUpdate(float Blend);

	/** Bound to the completion of WallClimbTimeline. Reset character movement mode to walking. */
	UFUNCTION()
	void WallClimbEnd();

	/** Callback function for the wall walking timeline, which acts as a wrapper for WallWalkUpdate(). 
	  * @param Value - The current value on the timeline.
	  */
	UFUNCTION()
	void WallClimbTimelineCallback(float Value);

	/* --- ROTATION MANAGEMENT --- */

	/** Sets character rotation to target rotation or interps toward target rotation. 
	  * @param NewTargetRotation - The desired rotation.
	  * @param ShouldRotationBeInterpolated - Whether rotation should be set instantly or moved to over a period of time.
	  * @param InterpSpeed - The speed at which interpolation should happen. The higher this is, the faster it occurs.
	  */
	void SetCharacterRotation(FRotator NewTargetRotation, bool ShouldRotationBeInterpolated, float InterpSpeed);

	/** Calls SetCharacterRotation() but will never rotate character greater than AimYawLimit. 
	  * @param AimYawLimit - The yaw rotation in local space which should never be exceeded.
	  * @param InterpSpeed - The speed at which interpolation should happen. The higher this is, the faster it occurs.
	  */
	void SetCharacterRotationWithLimit(float AimYawLimt, float InterpSpeed);

	/** Sets the rotation rate multiplier and returns the desired rotation rate. 
	  * @param SlowSpeed - The desired slowest speed at which the character rotates, in Unreal units.
	  * @param SlowSpeedRate - The desired slowest acceleration at which the character rotates, in Unreal units.
	  * @param FastSpeed - The desired fastest speed at which the character rotates, in Unreal units.
	  * @param FastSpeedRate - The desired fastest acceleration at which the character rotates, in Unreal units.
	  * @return The current rotation rate based on the passed params.
	  */
	float CalculateRotationRate(float SlowSpeed, float SlowSpeedRate, float FastSpeed, float FastSpeedRate);

	/** Returns the FRotator which determines character rotation based on the angles between the desired direction and cardinal direction. 
	  * @param OffsetInterpSpeed - The speed at which RotationOffset should be interpolated.
	  * @param NEAngle - The angle in local space which would rotate the character North-East.
	  * @param NWAngle - The angle in local space which would rotate the character North-West.
	  * @param SEAngle - The angle in local space which would rotate the character South-East.
	  * @param SWAngle - The angle in local space which would rotate the character South-West.
	  * @param Buffer - How much tolerance there is between the character and the desired looking rotation for each angle.
	  * @return The desired looking rotation.
	  */
	FRotator LookingDirectionWithOffset(float OffsetInterpSpeed, float NEAngle, float NWAngle, float SEAngle, float SWAngle, float Buffer);

	/** Returns true if the current cardinal direction is within a certain angle of CardinalDirectionToTest. Otherwise returns false.
	  * @param Value - A normalized delta rotator based on the character's current rotation.
	  * @param Min - The minimal angle (range).
	  * @param Max - The maximal angle (range).
	  * @param Buffer - How much tolerance there is above Max and below Min for the Cardinal Direction to be in range.
	  * @param CardinalDirectionToTest - The character's current cardinal direction.
	  * @return True if within the calculated range, false otherwise.
	  */
	bool IsPassedCardinalDirectionInRange(float Value, float Min, float Max, float Buffer, ECardinalDirection CardinalDirectionToTest);

	/** This function exists to prevent the ragdoll falling through the floor and adjusting the location/rotation of the capsule component.
	  * @param RagdollRotation - The ragdoll's current rotation with respect to the character's rotation.
	  * @param RagdollLocation - The ragdoll's current location with respect to the character's location.
	  * @param RagdollRotationOut - The ragdoll's adjusted rotation.
	  * @param RagdollLocationOut - The ragdoll's adjusted location.
	  */
	void CalculateActorLocationAndRotationWhileRagdolling(FRotator RagdollRotation, FVector CurrentRagdollLocation, FRotator& RagdollRotationOut, FVector& RagdollLocationOut);

	/* --- TICK FUNCTIONS --- */

	/** Calculates various input and rotation variables. */
	void CalculateEssentialVariables();

	/** Sets Max Acceleration and Ground Friction based on MovementInputVelocityDifference. */
	void CustomAcceleration();  

	/** Manages character rotation based on whether falling or grounded. */
	void ManageCharacterRotation();

	/** Tests if player can sprint and calls SetGait if they can. */
	void SprintCheck();

	/** Returns true if player can sprint. They cannot only if their movement input is not facing forward, to a 50 degree threshold. 
	  * @return Whether or not the character can sprint.
	  */
	bool CanSprint();

	/* --- TIMER RELATED --- */

	/** Called by LandedTimerHandle. */
	UFUNCTION()
	void OnLandedTimerHandleEnd();
};
