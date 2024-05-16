// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Autobattler.generated.h"

class AAutobattlerAIController;
class AAutobattlerCharacter;

/**
 * Base class for all autobattler decorators.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UBTDecorator_Autobattler : public UBTDecorator
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// BT DECORATOR INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Returns decorator condition.
	 * @param OwnerComp Used to get AI controller and pawn.
	 * @param NodeMemory Unused.
	 * @return Decorator condition.
	 */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	/**
	 * Autobattler version of decorator condition.
	 * @param OwnerComp BT owning this task.
	 * @param AIController AI Controller of BT.
	 * @param ControlledCharacter Character being moved by controller.
	 * @return Decorator condition.
	 */
	virtual bool AutobattlerCalculateDecoratorCondition(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* AIController, AAutobattlerCharacter* ControlledCharacter) const;
};
