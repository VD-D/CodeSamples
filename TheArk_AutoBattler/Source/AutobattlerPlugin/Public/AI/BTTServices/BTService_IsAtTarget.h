// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTServices/BTService_Autobattler.h"
#include "BTService_IsAtTarget.generated.h"

/**
 * Service which continually checks if target is within range of its target.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UBTService_IsAtTarget : public UBTService_Autobattler
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* (IN) Targeting mode we are using. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetingModeKey;

	/* (IN) The target we want to check distance to. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetKeyActor;

	/* (IN) The target we want to check distance to. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetKeyLocation;

	/* (IN) Radius to be considered "within distance". */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector RadiusKey;

	/* (OUT) Whether we are within the goal range or not. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector IsAtGoalKey;

	/* Whether we should set this AI's focus to the target. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	bool TrySetFocus;

/////////////////////////////////////////////////////////////////////////////////
//// BT SERVICE INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Constructor.
	 */
	UBTService_IsAtTarget();

	/**
	 * Checks if Controlled Character is within Radius.
	 * @param OwnerController Unused.
	 * @param ControlledCharacter Checked if within range of character.
	 * @param DeltaSeconds Unused.
	 */
	virtual void TickAutobattlerNode(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter, float DeltaSeconds) override;
};
