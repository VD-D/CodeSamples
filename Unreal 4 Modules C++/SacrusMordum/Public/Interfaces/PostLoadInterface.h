// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PostLoadInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPostLoadInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SACRUSMORDUM_API IPostLoadInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
