// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/EQS/EnvQueryContext_AtbTarget.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Types/AutobattlerStructs.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UEnvQueryContext_AtbTarget::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
    Super::ProvideContext(QueryInstance, ContextData);
    
    AAutobattlerAIController* QuerierAIController = Cast<AAutobattlerAIController>((Cast<AActor>((QueryInstance.Owner).Get())->GetInstigatorController()));
    if (!IsValid(QuerierAIController)) 
    {
        UAutobattlerFunctionLibrary::PrintErrorToLog(FString("[UEnvQueryContext_AtbTarget] Query Object could not be cast to Autobattler AI Controller!"));
        return;
    }

    FAbilityTargetingProperties AbilityTargetingProps;
    QuerierAIController->GetCurrentTargetingProperties(AbilityTargetingProps);

    if (AbilityTargetingProps.TargetingMode == ESkillTargetingMode::Actor)
    {
        UEnvQueryItemType_Actor::SetContextHelper(ContextData, AbilityTargetingProps.TargetCharacter);
    }
    else if (AbilityTargetingProps.TargetingMode == ESkillTargetingMode::Location)
    {
        UEnvQueryItemType_Point::SetContextHelper(ContextData, AbilityTargetingProps.TargetLocation);
    }
}
