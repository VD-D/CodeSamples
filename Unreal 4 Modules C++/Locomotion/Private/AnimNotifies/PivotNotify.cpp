// Copyright Robert Zygmunt Uszynski 2019-2020

#include "..\..\Public\AnimNotifies\PivotNotify.h"
#include "Character/LocomotionAnimInstance.h"

void UPivotNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ULocomotionAnimInstance* AnimInstanceReference = Cast<ULocomotionAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimInstanceReference)
	{
		AnimInstanceReference->SetPivotParameters(PivotParametersToSet);
	}
}