// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextDisplayWidget.generated.h"

/* A simple widget which exists to display text. */
UCLASS()
class SACRUSMORDUM_API UTextDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Text Display Widget")
	FText DisplayText;
};
