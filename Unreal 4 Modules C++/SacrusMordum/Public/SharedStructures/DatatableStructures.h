// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DatatableStructures.generated.h"

/* Contains all datatable structures and enums for Sacrus Mordum. */

USTRUCT(BlueprintType)
struct FProgression : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* The progression this level or sublevel is unlocked at. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 ProgressionStage;

	/* The name of the level. This may need to be repeated if different sublevels are unlocked at different
	progression stages. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	FName LevelName;

	/* Names of the sublevels unlocked at this progression stage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TArray<FName> SublevelNames;
};
