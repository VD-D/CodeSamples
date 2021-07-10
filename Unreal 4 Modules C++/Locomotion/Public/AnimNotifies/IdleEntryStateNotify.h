// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Enumerations/IdleEntryState.h"
#include "IdleEntryStateNotify.generated.h"

/* Notify which sets the state ULocomotionAnimInstance should be in
when entering idle. */
UCLASS()
class LOCOMOTION_API UIdleEntryStateNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	/* Override of Notify which sets IdleEntryStateToSet on ULocomotionAnimInstance. */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	EIdleEntryState IdleEntryStateToSet;
};
