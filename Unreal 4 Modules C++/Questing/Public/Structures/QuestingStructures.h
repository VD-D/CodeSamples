// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestingStructures.generated.h"

USTRUCT(BlueprintType)
struct FQuest : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Subobjectives to this quest. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Subobjectives;

	/* A brief description of this quest. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText QuestDescription;

	/* Optional ID for this quest. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 QuestID;

	/* How many stages this quest has. For example, a quest to find 4 items would have QuestStages = 4. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1"))
	uint8 QuestStages;

	/* True if subobjectives must be completed in order, false if objectives can be completed in any order. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequiresObjectivesToBeCompletedInOrder;

	bool bHasSubobjectives;
	uint8 ProgressedStages;

	int32 NumSubobjectives;
	int32 CompletedSubobjectives;

	FName ParentQuestName;

	FQuest()
	{
		QuestDescription = FText::FromName("Sample description of a quest");
		QuestID = 0;
		QuestStages = 1;
		bRequiresObjectivesToBeCompletedInOrder = true;
	}
};
