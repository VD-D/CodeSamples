// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/AutobattlerSkill.h"
#include "AutobattlerAbility.generated.h"

class UAutobattlerChargeComponent;

UENUM(BlueprintType)
enum class EAbilityTriggerType : uint8
{
	Cooldown UMETA(DisplayName = "Cooldown"),
	Charge   UMETA(DisplayName = "Charge"),
	Count    UMETA(Hidden)
};

/**
 * Definition for an Autobattler character ability.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerAbility : public UAutobattlerSkill
{
	GENERATED_BODY()
public:
	/* Whether this ability requires charges or triggers off cooldown. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	EAbilityTriggerType AbilityTriggerType;

	/* If the ability should be used immediately when the battle starts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation", meta = (EditCondition = "AbilityTriggerType==EAbilityTriggerType::Cooldown"))
	bool StartOffCooldown = false;

	/* How long before we can use the skill again. A cooldown of 0 or less means the character will *always* use their ability (and never attack). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation", meta = (EditCondition = "AbilityTriggerType==EAbilityTriggerType::Cooldown"))
	float Cooldown = 5.0f;

	/* The component class which determines how charges are gained. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation", meta = (EditCondition = "AbilityTriggerType==EAbilityTriggerType::Charge"))
	TSubclassOf<UAutobattlerChargeComponent> ChargeComponentClass;

	/* User-friendly description of how the character gains charges (e.g. "On Hit"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation", meta = (EditCondition = "AbilityTriggerType==EAbilityTriggerType::Charge"))
	FText ChargeGainDescription;

	/* How many charges are required to trigger the skill. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation", meta = (EditCondition = "AbilityTriggerType==EAbilityTriggerType::Charge", ClampMin = "1"))
	int32 ChargesNeededForTrigger;

	/* Only relevant if this is a skill, not an attack. Whether filtering should only considered characters already in range
	(i.e. will not move to a possible target if outside range). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Implementation")
	bool FilterCharactersNotInRange = false;
};
