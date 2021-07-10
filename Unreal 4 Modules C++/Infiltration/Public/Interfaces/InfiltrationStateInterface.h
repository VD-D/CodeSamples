// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InfiltrationStateInterface.generated.h"

UENUM(BlueprintType)
enum class EInfiltrationState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Patrolling UMETA(DisplayName = "Patrolling"),
	Alert UMETA(DisplayName = "Alert"),
	Cowering UMETA(DisplayName = "Cowering"),
	Fleeing UMETA(DisplayName = "Fleeing"),
	Searching UMETA(DisplayName = "Searching"),
	Chasing UMETA(DisplayName = "Chasing"),
	Attacking UMETA(DisplayName = "Attacking"),
	Dead UMETA(DisplayName = "Dead")
};

UINTERFACE(MinimalAPI)
class UInfiltrationStateInterface : public UInterface
{
	GENERATED_BODY()
};

/* Interface for passing around "states" between different AI actors. */
class INFILTRATION_API IInfiltrationStateInterface
{
	GENERATED_BODY()

public:
	/**
	 * Passes a list of strings defining the "states" an actor implementing this function has.
	 * States are highly interpretable, meaning that a state could be "IsPlayer" or equally "IsWalking". 
	 * These are meant to be interpreted by the AI of the Infiltration system.
	 * @param CurrentStates - (mutable) The current states of the implementing actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Infiltration States")
	void GetStates(TArray<FString>& CurrentStates);

	/**
	 * Indicates that the AI's state should be updated.
	 * @param NewState - The state which has been changed.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Infiltration States")
	void UpdateState(EInfiltrationState NewState);
};
