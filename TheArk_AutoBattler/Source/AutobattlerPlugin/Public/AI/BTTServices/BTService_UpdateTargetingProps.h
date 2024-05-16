// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTServices/BTService_Autobattler.h"
#include "BTService_UpdateTargetingProps.generated.h"

/**
 * Updates both current and previous targeting properties.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UBTService_UpdateTargetingProps : public UBTService_Autobattler
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* (OUT) Latest updated targeting mode. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CurrentTargetingModeKey;

	/* (OUT) Previous cycle targeting mode. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector PreviousTargetingModeKey;

	/* (OUT) Range of last updated skill. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CurrentRangeKey;

	/* (OUT) Previous cycle range. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector PreviousRangeKey;

	/* (OUT) Actor target of last updated skill. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CurrentActorTargetKey;

	/* (OUT) Previous cycle actor target. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector PreviousActorTargetKey;

	/* (OUT) Vector target of last updated skill. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CurrentLocationTargetKey;

	/* (OUT) Previous cycle vector target. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector PreviousLocationTargetKey;

/////////////////////////////////////////////////////////////////////////////////
//// BT SERVICE INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Constructor.
	 */
	UBTService_UpdateTargetingProps();

	/**
	 * Updates previous and current targeting settings.
	 * @param OwnerController Gets targeting settings.
	 * @param ControlledCharacter Unused.
	 * @param DeltaSeconds Unused.
	 */
	virtual void TickAutobattlerNode(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter, float DeltaSeconds) override;
};
