// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

// General includes.
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// UClass includes
#include "Camera/FixedCameraDelegator.h"

// Generated include.
#include "SacrusMordumFunctionLibrary.generated.h"

/* Function library for using in C++ and Blueprints for the SacrusMordumGame. */
UCLASS()
class SACRUSMORDUM_API USacrusMordumFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
/* --- FUNCTIONS --- */
public:
	/** Finds the fixed camera delegator by given name. If there are delegators with the same name, it will return
	  * the first one found (indeterminite)
	  * @param CameraDelegatorName - Name of the fixed camera delegator to find.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum", meta = (WorldContext = "WorldContextObject"))
	static AFixedCameraDelegator* GetCameraDelegatorByName(UObject* WorldContextObject, FName CameraDelegatorName);
};
