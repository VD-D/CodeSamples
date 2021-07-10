// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InfiltrationStateInterface.h"
#include "InfiltrationAIComponent.generated.h"

/* This component should be given to any actor who uses the Infiltration AI system. */
UCLASS( ClassGroup=(InfiltrationAI), meta=(BlueprintSpawnableComponent) )
class INFILTRATION_API UInfiltrationAIComponent : public UActorComponent, public IInfiltrationStateInterface
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- INITIAL BLACKBOARD VALUES --- */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration AI Component|Blackboard Values")
	TMap<FName, float> FloatValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration AI Component|Blackboard Values")
	TMap<FName, FVector> VectorValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration AI Component|Blackboard Values")
	TMap<FName, bool> BoolValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration AI Component|Blackboard Values")
	TMap<FName, FString> StringValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration AI Component|Blackboard Values")
	TMap<FName, class UObject*> ObjectValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration AI Component|Blackboard Values")
	bool bSetValuesOnBeginPlay;

	/* Whether or not to display on screen debug messages for this component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration AI Component|Debug")
	bool bEnableDebugMessages;

	/* The states currently stored by this component. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration AI Component|States")
	TArray<FString> CurrentStates;

/* --- FUNCTIONS --- */
public:	
	/**
	 * Constructor
	 */
	UInfiltrationAIComponent();

	/**
	 * Implementation of GetStates from InfiltrationStateInterface.
	 * @param States - (mutable) The current states. By default, returns CurrentStates.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Infiltration AI Component|States")
	void GetStates(TArray<FString>& States);
	virtual void GetStates_Implementation(TArray<FString>& States) override;

	/**
	 * If the target actor has an Infiltration AI Component, will test to see if their states
	 * contains the query string.
	 * @param Target - The target actor to test against. This should be a pawn.
	 * @param QueryString - The string to query against.
	 */
	bool QueryState(class AActor* Target, FString QueryString);

protected:
	/**
	 * Tries to set initial values on owning pawn.
	 */
	virtual void BeginPlay() override;
};
