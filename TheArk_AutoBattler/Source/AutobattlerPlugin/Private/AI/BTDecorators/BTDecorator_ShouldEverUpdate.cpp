// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/BTDecorators/BTDecorator_ShouldEverUpdate.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"

bool UBTDecorator_ShouldEverUpdate::AutobattlerCalculateDecoratorCondition(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* AIController, AAutobattlerCharacter* ControlledCharacter) const
{
    if (!IsValid(AIController)) return false;
    return AIController->GetAIShouldEverUpdate();
}
