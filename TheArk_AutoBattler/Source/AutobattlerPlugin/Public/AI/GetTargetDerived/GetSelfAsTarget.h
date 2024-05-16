// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AI/GetTarget.h"
#include "GetSelfAsTarget.generated.h"

/**
 * Returns OwningCharacter as the target.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UGetSelfAsTarget : public UGetTarget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// GET TARGET
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Implementation of GetTarget which returns the OwningCharacter as the target.
	 * @param OwningCharacter The character who "owns" the skill or attack.
	 * @param FilteredCharacters All characters which are possibly valid targets for the skill or attack (this depends on the skill and may result in this array being empty).
	 * @param TargetCharacter (OUT) The character who is the target of the skill.
	 * @param TargetLocation (OUT) For skills which need to use a location rather than a character.
	 * @return The targeting mode (actor, location or none).
	 */
	virtual ESkillTargetingMode GetTarget_Implementation(AAutobattlerCharacter* OwningCharacter, const TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter*& TargetCharacter, FVector& TargetLocation) override;
};
