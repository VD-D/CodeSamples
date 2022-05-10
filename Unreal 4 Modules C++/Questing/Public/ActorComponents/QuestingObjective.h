// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestingObjective.generated.h"


UCLASS(ClassGroup=(Questing), meta=(BlueprintSpawnableComponent))
class QUESTING_API UQuestingObjective : public UActorComponent
{
	GENERATED_BODY()

public:	
	/**
	 * Constructor.
	 */
	UQuestingObjective();

	/**
	 * Progresses an objective on a questing assigner.
	 * @param QuestingAssigner The assigner to progress the quest on.
	 * @param QuestName Name of the quest to progress.
	 * @param ProgressAmount Amount of stages to progress the quest. Clamped to minimum of 1.
	 */
	UFUNCTION(BlueprintCallable, Category = "Questing Objective")
	void ProgressQuest(class UQuestingAssigner* QuestingAssigner, const FName& QuestName, uint8 ProgressAmount = 1);

	/**
	 * Progresses an objective on ALL questing assigners spawned in the world.
	 * @param QuestName Name of the quest to progress.
	 * @param ProgressAmount Amount of stages to progress the quest. Clamped to minimum of 1.
	 */
	UFUNCTION(BlueprintCallable, Category = "Questing Objective")
	void ProgressQuestAll(const FName& QuestName, uint8 ProgressAmount = 1);
};
