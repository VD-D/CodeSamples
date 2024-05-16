// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EndSkill.generated.h"

/**
 * Animation notify which should be called when the state is ready to transition back to idle.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UEndSkill : public UAnimNotify
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// ANIMATION API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Animation notify which will go back through the chain of ownership (MeshComp -> AutobattlerCharacter -> AI Controller)
	 * to trigger a skill
	 * @param MeshComp Used to get the autobattler character owner
	 * @param Animation Used for debugging purposes.
	 */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
