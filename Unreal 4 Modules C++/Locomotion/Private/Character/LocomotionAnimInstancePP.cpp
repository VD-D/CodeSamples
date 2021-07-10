// Copyright Robert Zygmunt Uszynski 2019-2020

#include "..\..\Public\Character\LocomotionAnimInstancePP.h"
#include "..\..\Public\Character\LocomotionCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void ULocomotionAnimInstancePP::SetMovementType(EMovementType NewMovementType)
{
	MovementType = NewMovementType;
}

void ULocomotionAnimInstancePP::SetStance(EStance NewStance)
{
	Stance = NewStance;
}

void ULocomotionAnimInstancePP::InitAnimationPostProcess()
{
	CharacterReference = Cast<ALocomotionCharacter>(TryGetPawnOwner());
	if (CharacterReference)
	{
		ERotationMode DummyRotMode;
		EGait DummyGait;
		EMovementType DummyMovementType;
		bool DummyIsAiming;
		CharacterReference->GetMovementStates(DummyRotMode, DummyGait, MovementType, DummyMovementType, Stance, DummyIsAiming);
	}
	bEnableFootIK = true;
	LeftFootOffset = FVector(0.0f, 0.0f, 0.0f);
	RightFootOffset = FVector(0.0f, 0.0f, 0.0f);
}

void ULocomotionAnimInstancePP::UpdateAnimationPostProcess()
{
	switch (MovementType)
	{
	case EMovementType::Grounded:
		UpdateFootInverseKinematics();
		break;

	case EMovementType::Falling:
		bEnableFootIK = false;
		break;

	case EMovementType::Ragdoll:
		UpdateRagdollInverseKinematics();
		break;
	}
}

void ULocomotionAnimInstancePP::UpdateFootInverseKinematics()
{
	if (!bEnableFootIK)
	{
		bEnableFootIK = true;
	}

	FVector LocalOwnerLocation = GetOwningComponent()->GetComponentLocation();
	FVector LocalLeftFootLocation = GetOwningComponent()->GetSocketLocation(LeftFootBoneName);
	FVector LocalRightFootLocation = GetOwningComponent()->GetSocketLocation(RightFootBoneName);

	FVector LocalLeftFootStartTrace = FVector(LocalLeftFootLocation.X, LocalLeftFootLocation.Y, LocalOwnerLocation.Z + TraceLengthAboveFoot);
	FVector LocalLeftFootEndTrace = FVector(LocalLeftFootLocation.X, LocalLeftFootLocation.Y, LocalOwnerLocation.Z - TraceLengthBelowFoot);
	FVector LocalRightFootStartTrace = FVector(LocalRightFootLocation.X, LocalRightFootLocation.Y, LocalOwnerLocation.Z + TraceLengthAboveFoot);
	FVector LocalRightFootEndTrace = FVector(LocalRightFootLocation.X, LocalRightFootLocation.Y, LocalOwnerLocation.Z - TraceLengthBelowFoot);

	FHitResult LeftFootHitResult;
	FHitResult RightFootHitResult;
	bool bLeftFootHit = GetWorld()->LineTraceSingleByChannel(LeftFootHitResult, LocalLeftFootStartTrace, LocalLeftFootEndTrace, ECC_Visibility);
	bool bRightFootHit = GetWorld()->LineTraceSingleByChannel(RightFootHitResult, LocalRightFootStartTrace, LocalRightFootEndTrace, ECC_Visibility);

	if (bLeftFootHit)
	{
		LeftFootOffsetTargets = CalculatedClampedOffsets(UKismetMathLibrary::DegAtan2(LeftFootHitResult.Normal.Y, LeftFootHitResult.Normal.Z), UKismetMathLibrary::DegAtan2(LeftFootHitResult.Normal.X, LeftFootHitResult.Normal.Z), (LeftFootHitResult.Location.Z - LocalOwnerLocation.Z));
	}
	else
	{
		LeftFootOffsetTargets = FVector(0.0f, 0.0f, 0.0f);
	}

	if (bRightFootHit)
	{
		RightFootOffsetTargets = CalculatedClampedOffsets(UKismetMathLibrary::DegAtan2(RightFootHitResult.Normal.Y, RightFootHitResult.Normal.Z), UKismetMathLibrary::DegAtan2(RightFootHitResult.Normal.X, RightFootHitResult.Normal.Z), (RightFootHitResult.Location.Z - LocalOwnerLocation.Z));
	}
	else
	{
		RightFootOffsetTargets = FVector(0.0f, 0.0f, 0.0f);
	}

	LeftFootOffset = InterpolateOffset(LeftFootOffset, LeftFootOffsetTargets);
	RightFootOffset = InterpolateOffset(RightFootOffset, RightFootOffsetTargets);

	if (LeftFootOffset.Z < RightFootOffset.Z)
	{
		PelvisOffset = LeftFootOffset.Z;
	}
	else
	{
		PelvisOffset = RightFootOffset.Z;
	}
}

void ULocomotionAnimInstancePP::UpdateRagdollInverseKinematics()
{
	if (!bEnableFootIK)
	{
		bEnableFootIK = true;
	}

	LeftFootTransform.SetLocation(GetOwningComponent()->GetSocketLocation(LeftFootBoneName));
	LeftFootTransform.SetRotation(FQuat(GetOwningComponent()->GetSocketRotation(LeftFootBoneName)));
	LeftFootTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

	RightFootTransform.SetLocation(GetOwningComponent()->GetSocketLocation(RightFootBoneName));
	RightFootTransform.SetRotation(FQuat(GetOwningComponent()->GetSocketRotation(RightFootBoneName)));
	RightFootTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
}

FVector ULocomotionAnimInstancePP::CalculatedClampedOffsets(float Roll, float Pitch, float Yaw)
{
	float ReturnX = 0.0f;
	float ReturnY = 0.0f;
	float ReturnZ = 0.0f;
	if (Stance == EStance::Standing)
	{
		ReturnX = UKismetMathLibrary::FClamp(Roll, MinOffsetStanding.X, MaxOffsetStanding.X);
		ReturnY = -1.0f * UKismetMathLibrary::FClamp(Pitch, MinOffsetStanding.Y, MaxOffsetStanding.Y);
		ReturnZ = UKismetMathLibrary::FClamp(Yaw, MinOffsetStanding.Z, MaxOffsetStanding.Z);
	}
	else if (Stance == EStance::Crouching)
	{
		ReturnX = UKismetMathLibrary::FClamp(Roll, MinOffsetCrouching.X, MaxOffsetCrouching.X);
		ReturnY = -1.0f * UKismetMathLibrary::FClamp(Pitch, MinOffsetCrouching.Y, MaxOffsetCrouching.Y);
		ReturnZ = UKismetMathLibrary::FClamp(Yaw, MinOffsetCrouching.Z, MaxOffsetCrouching.Z);
	}
	return FVector(ReturnX, ReturnY, ReturnZ);
}

FVector ULocomotionAnimInstancePP::InterpolateOffset(FVector Current, FVector Target)
{
	FVector2D ReturnXY = UKismetMathLibrary::Vector2DInterpTo(FVector2D(Current.X, Current.Y), FVector2D(Target.X, Target.Y), GetWorld()->GetDeltaSeconds(), RotationOffsetInterpSpeed);
	float ReturnZ = UKismetMathLibrary::FInterpTo(Current.Z, Target.Z, GetWorld()->GetDeltaSeconds(), ZOffsetInterpSpeed);
	return FVector(ReturnXY.X, ReturnXY.Y, ReturnZ);
}
