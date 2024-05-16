// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_AtbAIPawn.generated.h"

/**
 * EQS context which returns the pawn owned by the Autobattler AI Controller.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UEnvQueryContext_AtbAIPawn : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
	/**
	 * Gets the last targeted actor of the querier (assuming they are an Autobattler character)
	 * @param QueryInstance Tries to get owner.
	 * @param ContextData Updates with last target actor.
	 */
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
