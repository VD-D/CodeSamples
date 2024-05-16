// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/EQS/EnvQueryContext_AtbAIPawn.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_AtbAIPawn::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
    Super::ProvideContext(QueryInstance, ContextData);

    AAutobattlerAIController* QuerierAIController = Cast<AAutobattlerAIController>((Cast<AActor>((QueryInstance.Owner).Get())->GetInstigatorController()));
    if (!IsValid(QuerierAIController)) 
    {
        UAutobattlerFunctionLibrary::PrintErrorToLog(FString("[UEnvQueryContext_AtbAIPawn] Query Object could not be cast to Autobattler AI Controller!"));
        return;
    }

    UEnvQueryItemType_Actor::SetContextHelper(ContextData, QuerierAIController->GetPawn());
}
