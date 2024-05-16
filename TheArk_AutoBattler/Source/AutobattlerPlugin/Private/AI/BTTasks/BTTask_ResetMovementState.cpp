// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/BTTasks/BTTask_ResetMovementState.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"

EBTNodeResult::Type UBTTask_ResetMovementState::ExecuteAutobattlerTask(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter)
{
    if (!IsValid(OwnerController)) return EBTNodeResult::Type::Failed;

    OwnerController->ResetMovementState();
    return EBTNodeResult::Type::Succeeded;
}
