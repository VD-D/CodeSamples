// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHighlightableInterface : public UInterface
{
	GENERATED_BODY()
};

/* Interface for enabling/disabling highlightables. */
class SACRUSMORDUM_API IHighlightableInterface
{
	GENERATED_BODY()

public:
	/** Call to enable/disable highlights.
	  * @parma Enable - Should the highlights be enabled/disabled?
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mouse Selectable Interface")
	void SetEnableHighlights(bool Enable);
};
