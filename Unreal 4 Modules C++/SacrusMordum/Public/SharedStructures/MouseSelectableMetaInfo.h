// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "MouseSelectableMetaInfo.generated.h"

USTRUCT(BlueprintType)
struct FMouseSelectableMetaInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Selectable Meta Info")
	FName SelectableItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Selectable Meta Info")
	int32 SelectableItemID;
};

