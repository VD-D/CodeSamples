// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/BTTasks/BTTask_ExecuteSkill.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"

UBTTask_ExecuteSkill::UBTTask_ExecuteSkill()
{
    NodeName = "Execute Skill";
}

EBTNodeResult::Type UBTTask_ExecuteSkill::ExecuteAutobattlerTask(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter)
{
    if (!IsValid(OwnerController)) return EBTNodeResult::Type::Failed;

    OwnerController->TryExecuteRelevantSkill();
    return EBTNodeResult::Type::Succeeded;
}
