// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Types/AutobattlerEnums.h"
#include "TriggerSkill.generated.h"

/**
 * Animation notify responsible for triggering a skill or attack on a character.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UTriggerSkill : public UAnimNotify
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// ANIMATION API
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Whether this animation triggers an attack or a skill. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	EAbilityType AbilityTypeTriggered;

	/**
	 * Animation notify which will go back through the chain of ownership (MeshComp -> AutobattlerCharacter -> AI Controller)
	 * to trigger a skill
	 * @param MeshComp Used to get the autobattler character owner
	 * @param Animation Used for debugging purposes.
	 */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
