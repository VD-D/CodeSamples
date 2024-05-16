// Copyright Juggler Games 2022 - 2024
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AutobattlerPlayerAbilityAsset.generated.h"

class APlayerExecuteAbility;
class UPlayerAbilityGetTarget;
class USkillImplementation;

/**
 * Data asset which contains player ability implementations.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerPlayerAbilityAsset : public UDataAsset
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// DATA
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Class for executing ability, which should vary depending on special effects. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	TSubclassOf<APlayerExecuteAbility> PlayerExecuteAbilityClass;

	/* Actual implementation of the skills. Note that the skill owner is always nullptr */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<USkillImplementation*> SkillEffects;

	/* What targets these skills can possibly affect (noting that they are still limited by targeting options of
	skills themselves). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UPlayerAbilityGetTarget* GetTargetImplementation;
};
