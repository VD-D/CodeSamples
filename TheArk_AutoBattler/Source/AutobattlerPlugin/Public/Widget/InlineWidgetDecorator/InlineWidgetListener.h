// Copyright Juggler Games 2024
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InlineWidgetListener.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInlineWidgetListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * Intended to be used by widgets used as InlineWidgetDecorator widgets or
 * widgets which are the parents of those widgets.
 */
class AUTOBATTLERPLUGIN_API IInlineWidgetListener
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Called when the inline widget is constructed.
	 * @param ConstructedWidget Widget which was just constructed.
	 * @param ParsedArgs Args passed to the widget in the tag.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inline Widget")
	void OnInlineWidgetConstructed(const TArray<FString>& ParsedArgs);
};
