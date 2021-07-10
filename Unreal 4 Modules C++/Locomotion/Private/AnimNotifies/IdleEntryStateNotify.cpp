// Copyright Robert Zygmunt Uszynski 2019-2020

#include "..\..\Public\AnimNotifies\IdleEntryStateNotify.h"
#include "Character/LocomotionAnimInstance.h"

void UIdleEntryStateNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ULocomotionAnimInstance* AnimInstanceReference = Cast<ULocomotionAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimInstanceReference)
	{
		AnimInstanceReference->SetIdleEntryState(IdleEntryStateToSet);
	}
}
