// Copyright Robert Zygmunt Uszynski 2019-2020

#include "..\..\Public\AnimNotifies\TurnInPlaceNotifyState.h"
#include "Animation/AnimMontage.h"
#include "Character/LocomotionAnimInstance.h"
#include "Character/LocomotionCharacter.h"
#include "Curves/CurveFloat.h"
#include "Kismet/KismetMathLibrary.h"

void UTurnInPlaceNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ULocomotionAnimInstance* AnimInstanceReference = Cast<ULocomotionAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimInstanceReference && MontageToPlay)
	{
		AnimInstanceReference->NotifiedTurningInPlace(MontageToPlay, true, true, bIsRightTurn);
	}
}

void UTurnInPlaceNotifyState::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	ALocomotionCharacter* CharacterReference = Cast<ALocomotionCharacter>(MeshComp->GetOwner());
	ULocomotionAnimInstance* AnimInstanceReference = Cast<ULocomotionAnimInstance>(MeshComp->GetAnimInstance());

	if (CharacterReference && AnimInstanceReference && MontageToPlay && RotationCurve)
	{
		float fLocalPosition = RotationCurve->GetFloatValue(AnimInstanceReference->Montage_GetPosition(MontageToPlay));
		float fLocalPlayRate = RotationCurve->GetFloatValue(AnimInstanceReference->Montage_GetPosition(MontageToPlay) + (AnimInstanceReference->Montage_GetPlayRate(MontageToPlay) * FrameDeltaTime));
		CharacterReference->AddCharacterRotation(FRotator(0.0f, fLocalPlayRate - fLocalPosition, 0.0f));
		if (UKismetMathLibrary::NotEqual_VectorVector(CharacterReference->GetVelocity(), FVector(0.0f, 0.0f, 0.0f), 0.0001f))
		{
			AnimInstanceReference->Montage_Stop(0.2f, MontageToPlay);
		}
	}
}

void UTurnInPlaceNotifyState::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	ULocomotionAnimInstance* AnimInstanceReference = Cast<ULocomotionAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimInstanceReference)
	{
		AnimInstanceReference->NotifiedTurningInPlace(nullptr, false, false, false);
	}
}
