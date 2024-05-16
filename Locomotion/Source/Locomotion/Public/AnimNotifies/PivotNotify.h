// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Structures/PivotParameters.h"
#include "PivotNotify.generated.h"

/* Notify which sets PivotParameters on ULocomotionAnimInstance to ensure pivots 
are executed smoothly. */
UCLASS()
class LOCOMOTION_API UPivotNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FPivotParameters PivotParametersToSet;
};
