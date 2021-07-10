// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InfiltrationUtilityLibrary.generated.h"

/* Utility library for the infiltration module.  */
UCLASS()
class INFILTRATION_API UInfiltrationUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
/* --- FUNCTIONS --- */

public:
	/**
	 * Sets float values on blackboard used by an AI Infilatration Controller from a map.
	 * @param AIInfiltrationController - The AI Infiltration Controller which is queried for its Blackboard component.
	 * @param ValuesToSet - The values to set on the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category = "Infiltration Utility Library")
	static void SetFloatValuesOnControllerBB(class AAIInfiltrationController* AIInfiltrationController, const TMap<FName, float>& ValuesToSet);

	/**
	 * Sets vector values on blackboard used by an AI Infilatration Controller from a map.
	 * @param AIInfiltrationController - The AI Infiltration Controller which is queried for its Blackboard component.
	 * @param ValuesToSet - The values to set on the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category = "Infiltration Utility Library")
	static void SetVectorValuesOnControllerBB(class AAIInfiltrationController* AIInfiltrationController, const TMap<FName, FVector>& ValuesToSet);

	/**
	 * Sets bool values on blackboard used by an AI Infilatration Controller from a map.
	 * @param AIInfiltrationController - The AI Infiltration Controller which is queried for its Blackboard component.
	 * @param ValuesToSet - The values to set on the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category = "Infiltration Utility Library")
	static void SetBoolValuesOnControllerBB(class AAIInfiltrationController* AIInfiltrationController, const TMap<FName, bool>& ValuesToSet);

	/**
	 * Sets string values on blackboard used by an AI Infilatration Controller from a map.
	 * @param AIInfiltrationController - The AI Infiltration Controller which is queried for its Blackboard component.
	 * @param ValuesToSet - The values to set on the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category = "Infiltration Utility Library")
	static void SetStringValuesOnControllerBB(class AAIInfiltrationController* AIInfiltrationController, const TMap<FName, FString>& ValuesToSet);

	/**
	 * Sets object values on blackboard used by an AI Infilatration Controller from a map.
	 * @param AIInfiltrationController - The AI Infiltration Controller which is queried for its Blackboard component.
	 * @param ValuesToSet - The values to set on the blackboard.
	 */
	UFUNCTION(BlueprintCallable, Category = "Infiltration Utility Library")
	static void SetObjectValuesOnControllerBB(class AAIInfiltrationController* AIInfiltrationController, const TMap<FName, class UObject*>& ValuesToSet);

	/**
	 * Attempts to get the Infiltration AI component on an actor.
	 * @param Owner - The owning actor to test against.
	 * @return The infiltration AI component is present, or nullptr if not present.
	 */
	static class UInfiltrationAIComponent* GetInfiltrationAIComponent(class AActor* Owner);

	/**
	 * Attempts to get the AI Infiltration Controller being used by an actor.
	 * @param Owner - The owning pawn.
	 * @return The AI Infiltration Controller if it is being used by the pawn, or nullptr if either a player controlled pawn or not using the AI Infiltration Controller.
	 */
	static class AAIInfiltrationController* GetInfiltrationAIController(class AActor* Owner);
	
};
