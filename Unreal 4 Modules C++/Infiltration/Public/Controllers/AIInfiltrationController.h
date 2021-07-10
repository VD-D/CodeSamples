// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/InfiltrationStateInterface.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIInfiltrationController.generated.h"

/**
 * 
 */
UCLASS()
class INFILTRATION_API AAIInfiltrationController : public AAIController, public IInfiltrationStateInterface
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* Components. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Infiltration Controller Components")
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Infiltration Controller Components")
	class UAIPerceptionComponent* AIPeripheralPerception;

	class UAISenseConfig_Sight* AISightConfiguration;
	class UAISenseConfig_Sight* AIPeripheralSightConfiguration;
	class UAISenseConfig_Hearing* AIHearingConfiguration;

	/* Configurable. */

	/* Behavior Tree to initially run on this AI controller. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Configuration")
	class UBehaviorTree* BehaviorTreeAsset;

	/* The "team" this controller is on. It follows that:
	- Controllers of the same TeamID are friendly. 
	- Controllers of different TeamIDs are hostile. 
	- Controllers with TeamID in NeutralTeamIDs are neutral. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Configuration")
	uint8 TeamID;

	/* Controllers with TeamID in NeutralTeamIDs are considered neutral. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Configuration")
	TArray<uint8> NeutralTeamIDs;

	/* Whether or not to enable on screen debug messages. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Debug")
	bool bEnableDebugMessages;

	/* Blackboard values */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Blackboard Values")
	bool bSetValuesOnPossess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Blackboard Values")
	TMap<FName, float> FloatValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Blackboard Values")
	TMap<FName, FVector> VectorValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Blackboard Values")
	TMap<FName, bool> BoolValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Blackboard Values")
	TMap<FName, FString> StringValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|Blackboard Values")
	TMap<FName, class UObject*> ObjectValues;

	/* The states currently stored by this component. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infiltration Controller|States")
	TArray<FString> CurrentStates;

private:
	/* Behavior Tree Component used by this controller. */
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	/* Blackboard Component used by this controller. */
	class UBlackboardComponent* BlackboardComponent;

/* --- FUNCTIONS --- */
public:
	/**
	 * Constructor.
	 */
	AAIInfiltrationController();

	/**
	 * Getter for the behavior tree component of this controller.
	 * @return The behavior tree component.
	 */
	FORCEINLINE class UBehaviorTreeComponent* GetBehaviorTreeComponent() const { return BehaviorTreeComponent; }

	/**
	 * Getter for the blackboard component of this controller. 
	 * @return The blackboard component.
	 */
	FORCEINLINE class UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }

	/**
	 * Implementation of GetStates from InfiltrationStateInterface.
	 * @param States - (mutable) The current states. By default, returns CurrentStates.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Infiltration AI Component|States")
	void GetStates(TArray<FString>& States);
	virtual void GetStates_Implementation(TArray<FString>& States) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Infiltration AI Component|States")
	void UpdateState(EInfiltrationState NewState);
	virtual void UpdateState_Implementation(EInfiltrationState NewState) override;

	/**
	 * If the target actor has an Infiltration AI Component, will test to see if their states
	 * contains the query string.
	 * @param Target - The target actor to test against. This should be a pawn.
	 * @param QueryString - The string to query against.
	 */
	UFUNCTION(BlueprintCallable, Category = "Infiltration AI Component|States")
	bool QueryState(class AActor* Target, FString QueryString);

	/**
	 * Function called whenever this controller percieves a friendly.
	 * @param AIStimuli - The stimuli which prompted this AI to react. 
	 * @param PerceivedFriendly - The actor which was perceived as friendly.
	 * @param SensingComponent - The component which triggered the reaction.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Infiltration AI Component|Reactions")
	void ReactToFriendly(const TArray<FAIStimulus>& AIStimuli, class AActor* PerceivedFriendly, class UAIPerceptionComponent* SensingComponent);
	virtual void ReactToFriendly_Implementation(const TArray<FAIStimulus>& AIStimuli, class AActor* PerceivedFriendly, class UAIPerceptionComponent* SensingComponent);

	/**
	 * Function called whenever this controller percieves a neutral.
	 * @param AIStimuli - The stimuli which prompted this AI to react. 
	 * @param PerceivedNeutral - The actor which was perceived as neutral.
	 * @param SensingComponent - The component which triggered the reaction.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Infiltration AI Component|Reactions")
	void ReactToNeutral(const TArray<FAIStimulus>& AIStimuli, class AActor* PerceivedNeutral, class UAIPerceptionComponent* SensingComponent);
	virtual void ReactToNeutral_Implementation(const TArray<FAIStimulus>& AIStimuli, class AActor* PerceivedNeutral, class UAIPerceptionComponent* SensingComponent);

	/**
	 * Function called whenever this controller percieves a hostile.
	 * @param AIStimuli - The stimuli which prompted this AI to react. 
	 * @param PerceivedHostile - The actor which was perceived as hostile.
	 * @param SensingComponent - The component which triggered the reaction.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Infiltration AI Component|Reactions")
	void ReactToHostile(const TArray<FAIStimulus>& AIStimuli, class AActor* PerceivedHostile, class UAIPerceptionComponent* SensingComponent);
	virtual void ReactToHostile_Implementation(const TArray<FAIStimulus>& AIStimuli, class AActor* PerceivedHostile, class UAIPerceptionComponent* SensingComponent);

	/**
	 * Override from Generic Team Interface to get attitude:
	 * 1. If the other actor cannot be cast to IGenericTeamInterface, returns neutral.
	 * 2. Same TeamIDs means friendly.
	 * 3. If the TeamID is in NeutralTeamIDs, returns neutral.
	 * 4. Different TeamIDs means hostile.
	 */
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
	/**
	 * Override of OnPossess. Initialises Behavior Tree and Blackboard components.
	 * @param ControlledPawn - Unused. 
	 */
	virtual void OnPossess(class APawn* ControlledPawn) override;

private:
	/**
	 * Function bound to OnPerceptionUpdated from the AIPerception component.
	 * @param DetectedActors - Actors detected by the perception component.
	 */
	UFUNCTION()
	void OnAIPerceptionUpdated(const TArray<class AActor*>& DetectedActors);

	/**
	 * Function bound to OnPerceptionUpdated from the AIPeripheralPerception component.
	 * @param DetectedActors - Actors detected by the perception component.
	 */
	UFUNCTION()
	void OnAIPeripheralPerceptionUpdated(const TArray<class AActor*>& DetectedActors);
};
