// Copyright Robert Uszynski 2021

#include "ActorComponents/QuestingObjective.h"
#include "ActorComponents/QuestingAssigner.h"

UQuestingObjective::UQuestingObjective()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuestingObjective::ProgressQuest(UQuestingAssigner* QuestingAssigner, const FName& QuestName, uint8 ProgressAmount)
{
	if (QuestingAssigner == nullptr) return;
	uint8 ClampedProgressAmount = ProgressAmount < 1 ? 1 : ProgressAmount;

	QuestingAssigner->ProgressQuest(QuestName, ProgressAmount);
}

void UQuestingObjective::ProgressQuestAll(const FName& QuestName, uint8 ProgressAmount)
{
	for (TObjectIterator<UQuestingAssigner> AssignerIterator; AssignerIterator; ++AssignerIterator)
	{
		UQuestingAssigner* CurrentAssigner = *AssignerIterator;
		if (CurrentAssigner->GetWorld() != GetWorld()) continue;

		ProgressQuest(CurrentAssigner, QuestName, ProgressAmount);
	}
}
