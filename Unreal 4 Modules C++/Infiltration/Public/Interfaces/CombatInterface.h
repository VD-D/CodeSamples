// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/*  */
class INFILTRATION_API ICombatInterface
{
	GENERATED_BODY()

public:
	/**
	 * Executed whenever implementor wishes to attack.
	 * @param TargetToAttack - The target to attack.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat Interface")
	void Attack(class AActor* TargetToAttack);
};
