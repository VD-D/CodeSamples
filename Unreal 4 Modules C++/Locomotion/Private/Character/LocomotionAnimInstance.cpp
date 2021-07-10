// Copyright Robert Zygmunt Uszynski 2019-2020

#include "..\..\Public\Character\LocomotionAnimInstance.h"
#include "..\..\Public\Character\LocomotionCharacter.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void ULocomotionAnimInstance::SetActiveLocomotionState(EActiveLocomotionState NewActiveLocomotionState)
{
	ActiveLocomotionState = NewActiveLocomotionState;
}

void ULocomotionAnimInstance::SetRotationMode(ERotationMode NewRotationMode)
{
	RotationMode = NewRotationMode;
}

void ULocomotionAnimInstance::SetAiming(bool NewAiming)
{
	bIsAiming = NewAiming;
}

void ULocomotionAnimInstance::SetGait(EGait NewGait)
{
	Gait = NewGait;
}

void ULocomotionAnimInstance::SetMovementType(EMovementType NewMovementType)
{
	MovementTypePrevious = MovementType;
	MovementType = NewMovementType;
	switch (MovementType)
	{
	case EMovementType::Falling:
		NPlayRate = 0.0f;
		CPlayRate = 0.0f;
		bShouldTurnInPlace = false;
		break;

	case EMovementType::Ragdoll:
		Montage_Stop(0.2f);
		break;
	}
}

void ULocomotionAnimInstance::SetMovementDirection(EMovementDirection NewMovementDirection)
{
	MovementDirection = NewMovementDirection;
}

void ULocomotionAnimInstance::SetStance(EStance NewStance)
{
	Stance = NewStance;
	if (bTurningInPlace)
	{
		Montage_Stop(0.2f, ActiveTurnInPlaceMontage);
	}
}

void ULocomotionAnimInstance::SetIdleEntryState(EIdleEntryState NewIdleEntryState)
{
	IdleEntryState = NewIdleEntryState;
}

void ULocomotionAnimInstance::SetPivotParameters(FPivotParameters NewPivotParameters)
{
	PivotParams = NewPivotParameters;
}

void ULocomotionAnimInstance::SetUpperBodyLayeringIndex(int32 NewUpperBodyLayeringIndex)
{
	UpperBodyLayeringIndex = NewUpperBodyLayeringIndex;
}

void ULocomotionAnimInstance::SetFullBodyLayeringIndex(int32 NewFullBodyLayeringIndex)
{
	FullBodyLayeringIndex = NewFullBodyLayeringIndex;
}

void ULocomotionAnimInstance::SetUseHeadRotation(bool NewUseHeadRotation)
{
	bShouldUseHeadRotation = NewUseHeadRotation;
}

void ULocomotionAnimInstance::SetHeadLookAtLocation(FVector NewHeadLookAtLocation)
{
	HeadLookAtLocation = NewHeadLookAtLocation;
}

FWallClimbAssetData ULocomotionAnimInstance::GetWallClimbDataFromType(EWallClimbType WallClimbType)
{
	switch (WallClimbType)
	{
	case EWallClimbType::Fall:
		return WallClimbTwoHandedDefault;
		
	case EWallClimbType::High:
		return WallClimbTwoHandedDefault;

	case EWallClimbType::Low:
		return WallClimbOneHandedDefault;
	}
	return WallClimbTwoHandedDefault;
}

void ULocomotionAnimInstance::NotifiedTurningInPlace(UAnimMontage * TurnInPlaceMontage, bool ShouldTurnInPlace, bool TurningInPlace, bool TurningRight)
{
	ActiveTurnInPlaceMontage = TurnInPlaceMontage;
	bShouldTurnInPlace = ShouldTurnInPlace;
	bTurningInPlace = TurningInPlace;
	bTurningRight = TurningRight;
}

void ULocomotionAnimInstance::InitAnimation()
{
	ALocomotionCharacter* CharacterReference = Cast<ALocomotionCharacter>(TryGetPawnOwner());

	if (CharacterReference)
	{

		WalkingSpeed = CharacterReference->WalkingSpeed;
		RunningSpeed = CharacterReference->RunningSpeed;
		SprintingSpeed = CharacterReference->SprintingSpeed;
		CrouchingSpeed = CharacterReference->CrouchingSpeed;

		CharacterReference->GetMovementStates(RotationMode, Gait, MovementType, MovementTypePrevious, Stance, bIsAiming);

		AimOffset = FVector2D(0.0f, 0.0f);
		CurrentSpeed = 0.0f;
		GaitMultiplier = 0.0f;
		bTurningInPlace = false;
	}
}

void ULocomotionAnimInstance::UpdateAnimation(float DeltaTimeX)
{
	ALocomotionCharacter* CharacterReference = Cast<ALocomotionCharacter>(TryGetPawnOwner());
	if (IsValid(CharacterReference)) {

		UCharacterMovementComponent* CharacterMovementReference = CharacterReference->GetCharacterMovement();
		UCapsuleComponent* CapsuleComponentReference = CharacterReference->GetCapsuleComponent();

		CharacterReference->GetMovementProperties(Velocity, CharacterRotation, LastVelocityRotation, LastMovementInputRotation, LookingRotation, Direction, TargetCharacterRotationDifference, MovementInputVelocityDifference, AimYawDelta, AimYawRate, bIsMoving, bHasMovementInput);
		CalculateLookingAimOffset();
		CalculateHeadRotation();

		switch (MovementType)
		{
		case EMovementType::Grounded:
			CurrentSpeed = Velocity.Size();
			if (bIsMoving)
			{
				CalculateGaitMultiplier();
				CalculateAnimPlayRates(150.0f, 350.0f, 600.0f, 150.0f, CapsuleComponentReference);
				CalculateMovementDirection(-90.0f, 90.0f, 5.0f);
			}
			else
			{
				if (!(CharacterReference->IsPlayingRootMotion()) && RotationMode == ERotationMode::TargetDirection)
				{
					if (bIsAiming && Stance == EStance::Standing)
					{
						TurnInPlaceResponsive(60.0f, AimingTurnLeftNinetyDegrees, AimingTurnRightNinetyDegrees, 1.5f);
					}
					else if (bIsAiming && Stance == EStance::Crouching)
					{
						TurnInPlaceResponsive(60.0f, CrouchingTurnLeftNinetyDegrees, CrouchingTurnRightNinetyDegrees, 1.5f);
					}
					else if (!bIsAiming && Stance == EStance::Standing && !bTurningInPlace)
					{
						TurnInPlaceDelayed(100.0f, 60.0f, 0.5f, 1.5f, NeutralTurnLeftNinetyDegrees, NeutralTurnRightNinetyDegrees, 130.0f, 0.0f, 1.25f, NeutralTurnLeftNinetyDegrees, NeutralTurnRightHundredEightyDegrees);
					}
					else if (!bIsAiming && Stance == EStance::Crouching && !bTurningInPlace)
					{
						TurnInPlaceDelayed(100.0f, 60.0f, 0.5f, 1.5f, CrouchingTurnLeftNinetyDegrees, CrouchingTurnRightNinetyDegrees, 130.0f, 0.0f, 1.25f, CrouchingTurnLeftNinetyDegrees, CrouchingTurnRightNinetyDegrees);
					}
				}
			}

			if (ActiveLocomotionState == EActiveLocomotionState::NotMoving && bIsMoving)
			{
				if (Stance == EStance::Standing && bIsAiming && Direction > 0.0f)
				{
					StartPosition = 0.187f;
				}
				else if (Stance == EStance::Standing && bIsAiming && Direction <= 0.0f)
				{
					StartPosition = 1.0f;
				}
				else if (Stance == EStance::Standing && !bIsAiming && Direction > 0.0f)
				{
					StartPosition = 0.3f;
				}
				else if (Stance == EStance::Standing && !bIsAiming && Direction <= 0.0f)
				{
					StartPosition = 0.867f;
				}
				else if (Stance == EStance::Crouching && Direction > 0.0f)
				{
					StartPosition = 0.25;
				}
				else if (Stance == EStance::Crouching && Direction <= 0.0f)
				{
					StartPosition = 0.5f;
				}
				else
				{
					StartPosition = 0.0f;
				}
			}
			else if (ActiveLocomotionState == EActiveLocomotionState::Moving)
			{
				VelocityDifference = UKismetMathLibrary::NormalizedDeltaRotator(LastVelocityRotation, PreviousVelocityRotation).Yaw / GetWorld()->GetDeltaSeconds();
				PreviousVelocityRotation = LastVelocityRotation;
				float CurrentSpeedMultiplier = UKismetMathLibrary::MapRangeClamped(CurrentSpeed, WalkingSpeed, RunningSpeed, 0.0f, 1.0f);
				LeanRotation = UKismetMathLibrary::MapRangeClamped(VelocityDifference, -200.0f, 200.0f, -1.0f, 1.0f) * CurrentSpeedMultiplier;
				AccelerationDifference = (CurrentSpeed - PreviousSpeed) / GetWorld()->GetDeltaSeconds();
				PreviousSpeed = CurrentSpeed;
				if (AccelerationDifference > 0.0f)
				{
					LeanAcceleration = UKismetMathLibrary::MapRangeClamped(abs(AccelerationDifference), 0.0f, CharacterMovementReference->GetMaxAcceleration(), 0.0f, 1.0f) * CurrentSpeedMultiplier;
				}
				else
				{
					LeanAcceleration = UKismetMathLibrary::MapRangeClamped(abs(AccelerationDifference), 0.0f, CharacterMovementReference->GetMaxAcceleration(), 0.0f, 1.0f) * CurrentSpeedMultiplier;
				}
				FVector RotatedAroundZ = UKismetMathLibrary::RotateAngleAxis(FVector(LeanRotation, LeanAcceleration, 0.0f), Direction, FVector(0.0f, 0.0f, -1.0f));
				LeanGrounded.X = RotatedAroundZ.X;
				LeanGrounded.Y = RotatedAroundZ.Y;
			}
			else if (ActiveLocomotionState == EActiveLocomotionState::Pivot)
			{
				if (UKismetMathLibrary::NearlyEqual_FloatFloat(Direction, PivotParams.PivotDirection, 45.0f))
				{
					MovementDirection = PivotParams.InterruptedMovementDirection;
					StartPosition = PivotParams.InterruptedStartTime;
				}
				else
				{
					MovementDirection = PivotParams.CompletedMovementDirection;
					StartPosition = PivotParams.CompletedStartTime;
				}
			}
			break;

		case EMovementType::Falling:
			CurrentSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
			FlailBlendAlpha = FlailAlphaCurve->GetFloatValue(Velocity.Z * -1.0f);

			LeanInAir = UKismetMathLibrary::MapRangeClamped(Velocity.Z, CharacterMovementReference->JumpZVelocity, (CharacterMovementReference->JumpZVelocity * 2), 1.0f, -1.0f) * UKismetMathLibrary::NormalizeToRange(CurrentSpeed, 0.0f, RunningSpeed);

			if (Velocity.Z < 0.0f)
			{
				FHitResult Hit(ForceInit);
				FVector ControlledActorLocation = CharacterReference->GetActorLocation();
				FVector StartLocationocation = FVector(ControlledActorLocation.X, ControlledActorLocation.Y, ControlledActorLocation.Z - CapsuleComponentReference->GetScaledCapsuleHalfHeight());
				FVector EndLocation = FVector(Velocity.X, Velocity.Y, UKismetMathLibrary::FClamp(Velocity.Z, -4000.0f, -200.0f));
				UKismetMathLibrary::Vector_Normalize(EndLocation, 0.0001f);
				EndLocation = (EndLocation * UKismetMathLibrary::MapRangeClamped(Velocity.Z, 0.0f, -4000.0f, 50.0f, 2000.0f)) + StartLocationocation;
				bool bHit = GetWorld()->SweepSingleByChannel(Hit, StartLocationocation, EndLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(CapsuleComponentReference->GetScaledCapsuleRadius()));
				if (bHit && Hit.ImpactNormal.Z >= CharacterMovementReference->GetWalkableFloorZ())
				{
					if (LandAlphaCurve)
					{
						LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, LandAlphaCurve->GetFloatValue(UKismetMathLibrary::MapRangeClamped(Hit.Time, 0.0f, 1.0f, 1.0f, 0.0f)), GetWorld()->GetDeltaSeconds(), 20.0f);
					}
				}

				else
				{
					LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, 0.0f, GetWorld()->GetDeltaSeconds(), 10.0f);
				}
			}
			else
			{
				LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, 0.0f, GetWorld()->GetDeltaSeconds(), 10.0f);
			}
			break;

		case EMovementType::Ragdoll:
			FlailRate = UKismetMathLibrary::MapRangeClamped(Velocity.Size(), 0.0f, 1000.0f, 0.0f, 1.25f);
			break;
		}
	}
}

void ULocomotionAnimInstance::IdleTransition(UAnimSequenceBase * AnimationToPlay, float InPlayRate, float InTimeToStartMontage)
{
	PlaySlotAnimationAsDynamicMontage(AnimationToPlay, FName("Transition"), 0.2f, 0.2f, InPlayRate, 1, 0.0f, InTimeToStartMontage);
}

void ULocomotionAnimInstance::CalculateFeetPosition()
{
	FeetPosition.X = GetCurveValue(FootDirectionCurveName);
	FeetPosition.Y = GetCurveValue(FootPositionCurveName);
}

void ULocomotionAnimInstance::PlayMontageIfValid(UAnimMontage * Montage, FName SectionName)
{
	if (Montage)
	{
		float const A = Montage_Play(Montage);
		if (A > 0.0f && SectionName != NAME_None && Montage->IsValidSectionName(SectionName))
		{
			Montage_JumpToSection(SectionName, Montage);
		}
	}
}

void ULocomotionAnimInstance::CalculateLookingAimOffset()
{
	switch (RotationMode)
	{
	case ERotationMode::TargetDirection:
	{
		FRotator LocalDeltaCamera = UKismetMathLibrary::NormalizedDeltaRotator(LookingRotation, CharacterRotation);
		AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D(LocalDeltaCamera.Yaw, LocalDeltaCamera.Pitch),
			GetWorld()->GetDeltaSeconds(),
			UKismetMathLibrary::MapRangeClamped(abs(LocalDeltaCamera.Yaw - AimOffset.X), 0.0f, 180.0f, 30.0f, 5.0f));
		break;
	}

	case ERotationMode::VelocityDirection:
		if (bHasMovementInput)
		{
			FRotator LocalDeltaVelocity = UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, CharacterRotation);
			AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D(UKismetMathLibrary::FClamp(LocalDeltaVelocity.Yaw, -90.0f, 90.0f), LocalDeltaVelocity.Pitch),
				GetWorld()->GetDeltaSeconds(),
				UKismetMathLibrary::MapRangeClamped(abs(LocalDeltaVelocity.Yaw - AimOffset.X), 0.0f, 180.0f, 15.0f, 5.0f));
		}
		else
		{
			AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D(0.0f, 0.0f), GetWorld()->GetDeltaSeconds(), 4.0f);
		}
		break;

	default:
		AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D(0.0f, 0.0f), GetWorld()->GetDeltaSeconds(), 4.0f);
		break;
	}
}

void ULocomotionAnimInstance::CalculateHeadRotation()
{
	if (GetWorld() == nullptr) return;

	if (bShouldUseHeadRotation && GetOwningActor() != nullptr)
	{
		FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(GetOwningActor()->GetActorRotation(), UKismetMathLibrary::FindLookAtRotation(GetOwningActor()->GetActorLocation(), HeadLookAtLocation));
		HeadLookingRotation = UKismetMathLibrary::RInterpTo(HeadLookingRotation, FRotator(0.0f, FMath::Clamp<float>(Delta.Pitch, MaxHeadRotation * -1.0f, MaxHeadRotation), FMath::Clamp<float>(Delta.Yaw, MaxHeadRotation * -1.0f, MaxHeadRotation)), GetWorld()->GetDeltaSeconds(), HeadRotationInterpSpeed);
	}
	else if (!bShouldUseHeadRotation || GetOwningActor() == nullptr)
	{
		HeadLookingRotation = UKismetMathLibrary::RInterpTo(HeadLookingRotation, FRotator(0.0f, 0.0f, 0.0f), GetWorld()->GetDeltaSeconds(), HeadRotationInterpSpeed);
	}
	else
	{
		HeadLookingRotation = FRotator(0.0f, 0.0f, 0.0f);
	}
}

void ULocomotionAnimInstance::CalculateGaitMultiplier()
{
	if (CurrentSpeed > WalkingSpeed)
	{
		if (CurrentSpeed > RunningSpeed)
		{
			GaitMultiplier = UKismetMathLibrary::MapRangeClamped(CurrentSpeed, RunningSpeed, SprintingSpeed, 2.0f, 3.0f);
		}
		else
		{
			GaitMultiplier = UKismetMathLibrary::MapRangeClamped(CurrentSpeed, WalkingSpeed, RunningSpeed, 1.0f, 2.0f);
		}
	}
	else
	{
		GaitMultiplier = UKismetMathLibrary::MapRangeClamped(CurrentSpeed, 0.0f, WalkingSpeed, 0.0f, 1.0f);
	}
}

void ULocomotionAnimInstance::CalculateAnimPlayRates(float WalkAnimSpeed, float RunAnimSpeed, float SprintAnimSpeed, float CrouchAnimSpeed, UCapsuleComponent * CapsuleComponentReference)
{
	float LocalCapsuleComponentScale = CapsuleComponentReference->GetComponentScale().Z;
	if (GaitMultiplier > 2.0f)
	{
		NPlayRate = (UKismetMathLibrary::MapRangeClamped(GaitMultiplier, 2.0f, 3.0f, UKismetMathLibrary::MapRangeUnclamped(CurrentSpeed, 0.0f, RunAnimSpeed, 0.0f, 1.0f), UKismetMathLibrary::MapRangeUnclamped(CurrentSpeed, 0.0f, SprintAnimSpeed, 0.0f, 1.0f))) / LocalCapsuleComponentScale;
	}
	else
	{
		NPlayRate = (UKismetMathLibrary::MapRangeClamped(GaitMultiplier, 1.0f, 2.0f, UKismetMathLibrary::MapRangeUnclamped(CurrentSpeed, 0.0f, WalkAnimSpeed, 0.0f, 1.0f), UKismetMathLibrary::MapRangeUnclamped(CurrentSpeed, 0.0f, RunAnimSpeed, 0.0f, 1.0f))) / LocalCapsuleComponentScale;
	}
	CPlayRate = (UKismetMathLibrary::MapRangeUnclamped(CurrentSpeed, 0.0f, CrouchAnimSpeed, 0.0f, 1.0f)) / LocalCapsuleComponentScale;
}

void ULocomotionAnimInstance::CalculateMovementDirection(float DirectionThresholdMin, float DirectionThresholdMax, float Buffer)
{
	bool bLocalCondition;
	switch (MovementDirection)
	{
	case EMovementDirection::Forward:
		bLocalCondition = UKismetMathLibrary::InRange_FloatFloat(Direction, (DirectionThresholdMin - Buffer), (DirectionThresholdMax + Buffer));
		break;

	case EMovementDirection::Backward:
		bLocalCondition = UKismetMathLibrary::InRange_FloatFloat(Direction, (DirectionThresholdMin + Buffer), (DirectionThresholdMax - Buffer));
		break;

	default:
		bLocalCondition = true;
		break;
	}

	if (bLocalCondition)
	{
		MovementDirection = EMovementDirection::Forward;
	}
	else
	{
		MovementDirection = EMovementDirection::Backward;
	}
}

void ULocomotionAnimInstance::TurnInPlaceResponsive(float AimYawLimit, UAnimMontage * TurnLeftMontage, UAnimMontage * TurnRightMontage, float PlayRate)
{
	bShouldTurnInPlace = abs(AimYawDelta) > AimYawLimit;
	if (abs(AimYawDelta) > AimYawLimit)
	{
		bool bIsAimYawDeltaPositive = AimYawDelta > 0.0f;
		UAnimMontage* AnimMontageToPlay;
		float PlayRateResponsive = PlayRate * UKismetMathLibrary::MapRangeClamped(AimYawRate, 120.0f, 400.0f, 1.0f, 2.0f);

		if (bIsAimYawDeltaPositive)
		{
			AnimMontageToPlay = TurnRightMontage;
		}
		else
		{
			AnimMontageToPlay = TurnLeftMontage;
		}

		if (!bTurningInPlace && !Montage_IsPlaying(AnimMontageToPlay) && AnimMontageToPlay)
		{
			Montage_Play(AnimMontageToPlay, PlayRateResponsive);
		}
		else if (bTurningInPlace && ((bTurningRight && !(AimYawDelta > 0.0f)) || (!bTurningRight && AimYawDelta > 0.0f)) && !Montage_IsPlaying(AnimMontageToPlay) && AnimMontageToPlay)
		{
			Montage_Play(AnimMontageToPlay, PlayRateResponsive);
		}
	}
}

void ULocomotionAnimInstance::TurnInPlaceDelayed(float MaxCameraSpeed, float AimYawLimit1, float DelayTime1, float PlayRate1, UAnimMontage * TurnLeftMontage1, UAnimMontage * TurnRightMontage1, float AimYawLimit2, float DelayTime2, float PlayRate2, UAnimMontage * TurnLeftMontage2, UAnimMontage * TurnRightMontage2)
{
	if ((abs(AimYawRate) < MaxCameraSpeed) && (abs(AimYawDelta) > AimYawLimit1))
	{
		TurnInPlaceDelayCount = TurnInPlaceDelayCount + GetWorld()->GetDeltaSeconds();
		bShouldTurnInPlace = TurnInPlaceDelayCount > UKismetMathLibrary::MapRangeClamped(abs(AimYawDelta), AimYawLimit1, AimYawLimit2, DelayTime1, DelayTime2);
		UAnimMontage* AnimMontageToPlay;
		float PlayRateFinal = 0.0f;

		if ((AimYawDelta > 0.0f) && (abs(AimYawDelta) >= AimYawLimit2))
		{
			AnimMontageToPlay = TurnRightMontage2;
		}
		else if ((AimYawDelta > 0.0f) && (abs(AimYawDelta) < AimYawLimit2))
		{
			AnimMontageToPlay = TurnRightMontage1;
		}
		else if ((AimYawDelta <= 0.0f) && (abs(AimYawDelta) >= AimYawLimit2))
		{
			AnimMontageToPlay = TurnLeftMontage1;
		}
		else if ((AimYawDelta <= 0.0f) && (abs(AimYawDelta) < AimYawLimit2))
		{
			AnimMontageToPlay = TurnLeftMontage2;
		}
		else
		{
			AnimMontageToPlay = nullptr;
		}

		if (abs(AimYawDelta) >= AimYawLimit2)
		{
			PlayRateFinal = PlayRate2;
		}
		else
		{
			PlayRateFinal = PlayRate1;
		}

		if (AnimMontageToPlay)
		{
			if (!Montage_IsPlaying(AnimMontageToPlay))
			{
				Montage_Play(AnimMontageToPlay, PlayRateFinal);
			}
		}
	}
	else
	{
		TurnInPlaceDelayCount = 0.0f;
	}
}
