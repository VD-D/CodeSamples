// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/AutobattlerEnums.h"
#include "GetTarget.generated.h"

class AAutobattlerCharacter;

/**
 * Base class which allows for implementation of getting a target for a skill.
 * By default, we return a completely random target which is not the owning character.
 */
UCLASS(Blueprintable)
class AUTOBATTLERPLUGIN_API UGetTarget : public UObject
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// GET TARGET
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Default function intended to be overriden by child classes to get a target for a skill or attack.
	 * @param OwningCharacter The character who "owns" the skill or attack.
	 * @param FilteredCharacters All characters which are possibly valid targets for the skill or attack (this depends on the skill and may result in this array being empty).
	 * @param TargetActor (OUT) The actor who is the target of the skill.
	 * @param TargetLocation (OUT) For skills which need to use a location rather than a character.
	 * @return The targeting mode (actor, location or none).
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Get Target")
	ESkillTargetingMode GetTarget(AAutobattlerCharacter* OwningCharacter, const TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter*& TargetCharacter, FVector& TargetLocation);
	virtual ESkillTargetingMode GetTarget_Implementation(AAutobattlerCharacter* OwningCharacter, const TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter*& TargetCharacter, FVector& TargetLocation);
};
