// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Autobattler.generated.h"

class AAutobattlerAIController;
class AAutobattlerCharacter;

/**
 * Base service task used for autobattler services.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UBTService_Autobattler : public UBTService
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// BT SERVICE INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Ticks node.
	 * @param OwnerComp Used to get controller and pawn.
	 * @param NodeMemory Unused.
	 * @param DeltaSeconds Passed to service tick.
	 */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/**
	 * Autobattler tick implementation, to be overriden.
	 * @param OwnerComp BT owner component.
	 * @param OwnerController AI controller.
	 * @param ControlledCharacter Character controlled by AI.
	 * @param DeltaSeconds Time since last tick.
	 */
	virtual void TickAutobattlerNode(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter, float DeltaSeconds);
};
