// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Structures/QuestingStructures.h"
#include "QuestingInterface.generated.h"

UINTERFACE(MinimalAPI)
class UQuestingInterface : public UInterface
{
	GENERATED_BODY()
};

class QUESTING_API IQuestingInterface
{
	GENERATED_BODY()

public:
	/**
	 * Called whenever a quest begins.
	 * @param QuestInfo Information about the quest. 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Questing Interface")
	void OnQuestBegan(const FQuest& QuestInfo);

	/**
	 * Called whenever a quest progresses, but is not completed.
	 * @param QuestInfo Information about the quest. 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Questing Interface")
	void OnQuestProgressed(const FQuest& QuestInfo);

	/**
	 * Called whenever a quest finishes.
	 * @param QuestInfo Information about the quest. 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Questing Interface")
	void OnQuestFinished(const FQuest& QuestInfo);
};
