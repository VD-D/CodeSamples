// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_AtbTarget.generated.h"

/**
 * EQS context which gets the last targeted actor of the querier (assuming they are an Autobattler character).
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UEnvQueryContext_AtbTarget : public UEnvQueryContext
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
