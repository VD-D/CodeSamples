// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FocusInterface.generated.h"

UINTERFACE(MinimalAPI)
class UFocusInterface : public UInterface
{
	GENERATED_BODY()
};

/* Interface for focus events. */
class SACRUSMORDUM_API IFocusInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Focus Interface")
	void OnFocusBegan();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Focus Interface")
	void OnFocusInterrupted();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Focus Interface")
	void OnFocusCompleted();
};
