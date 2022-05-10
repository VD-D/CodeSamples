// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InvestigationDiscoveryInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInvestigationDiscoveryInterface : public UInterface
{
	GENERATED_BODY()
};

/* Interface for whenever the player uncovers investigation objects. */
class SACRUSMORDUM_API IInvestigationDiscoveryInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Investigation Discovery Interface")
	void OnInvestigationObjectFound(class AInvestigationObject* FoundObject);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Investigation Discovery Interface")
	void OnInvestigationObjectLost(class AInvestigationObject* LostObject);
};
