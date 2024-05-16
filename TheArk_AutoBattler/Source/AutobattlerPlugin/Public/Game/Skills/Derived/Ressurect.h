// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Skills/SkillImplementation.h"
#include "Ressurect.generated.h"

/**
 * Sets the target character or characters to be able to ressurect. Ressurection can only occur once.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API URessurect : public USkillImplementation
{
	GENERATED_BODY()
	
/////////////////////////////////////////////////////////////////////////////////
//// SKILL IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Deals damage to each of the targets.
	 * @param SkillOwner Unused.
	 * @param SkillTargetingMode This should be actor.
	 * @param Target Target to deal damage to.
	 * @param Location Unused. 
	 */
	virtual void ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation) override;
};
