// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TurnInPlaceNotifyState.generated.h"

/* Notify state which determines which montage needs to be played and which rotation curve
to use when rotating. */
UCLASS()
class LOCOMOTION_API UTurnInPlaceNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UCurveFloat* RotationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	bool bIsRightTurn;
};
