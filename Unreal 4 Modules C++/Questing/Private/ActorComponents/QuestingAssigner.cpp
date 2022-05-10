// Copyright Robert Uszynski 2021

#include "ActorComponents/QuestingAssigner.h"
#include "Engine/DataTable.h"
#include "Interfaces/QuestingInterface.h"

UQuestingAssigner::UQuestingAssigner()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuestingAssigner::AssignNewQuest(const FName& QuestRowName)
{
	AssignQuestInternal(QuestRowName);
}

void UQuestingAssigner::ProgressQuest(const FName& AssignedQuestName, uint8 NumStagesToProgress)
{
	if (!AssignedQuests.Contains(AssignedQuestName)) return;

	FQuest* QuestToProgress = AssignedQuests.Find(AssignedQuestName);
	if (QuestToProgress == nullptr) return;

	if (!QuestToProgress->bHasSubobjectives) 
	{
		if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
		FString::Printf(TEXT("Questing Assinger: Updated quest %s with %d stages"), *AssignedQuestName.ToString(), NumStagesToProgress));
		QuestToProgress->ProgressedStages += NumStagesToProgress;
	}

	if (QuestToProgress->ProgressedStages >= QuestToProgress->QuestStages) CompleteQuest(AssignedQuestName, QuestToProgress);
	else ExecuteQuestingInterface(EQuestingInterfaceFunction::Progressed, *QuestToProgress);
}

const TMap<FName, FQuest>& UQuestingAssigner::GetAssignedQuests()
{
	return AssignedQuests;
}

const TMap<FName, FName>& UQuestingAssigner::GetPendingSubobjectives()
{
	return PendingSubobjectives;
}

const TMap<FName, FQuest>& UQuestingAssigner::GetCompletedQuests()
{
	return CompletedQuests;
}

void UQuestingAssigner::AssignQuestInternal(const FName& QuestRowName, const FName& ParentQuest)
{
	/* Prevent already assigned quests on being reassigned. */
	if (AssignedQuests.Contains(QuestRowName) || CompletedQuests.Contains(QuestRowName)) return;

	/* Do nothing if the passed datatable is null. */
	if (QuestsDatatable == nullptr) 
	{
		if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, 
		FString("Questing Assinger: Cannot assign quest as datatable is null!"));
		return;
	}

	/* Try to find the quest in the datatable. Do nothing if it cannot be found. */
	FQuest* FoundQuest = QuestsDatatable->FindRow<FQuest>(QuestRowName, FString("Questing Datatable"));
	if (FoundQuest == nullptr)
	{
		if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, 
		FString::Printf(TEXT("Questing Assinger: Cannot find quest with name %s in datatable!"), *QuestRowName.ToString()));
		return;
	}

	/* Update internal variables in the struct. */
	FoundQuest->bHasSubobjectives = FoundQuest->Subobjectives.Num() > 0;
	FoundQuest->NumSubobjectives = FoundQuest->Subobjectives.Num();
	FoundQuest->CompletedSubobjectives = 0;
	FoundQuest->ParentQuestName = ParentQuest;

	/* Add the new quest to the presently assigned quests. */
	AssignedQuests.Add(QuestRowName, *FoundQuest);
	if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
	FString::Printf(TEXT("Questing Assinger: Successfully assigned new quest %s"), *QuestRowName.ToString()));

	if (FoundQuest->Subobjectives.Num() > 0)
	{
		if (FoundQuest->bRequiresObjectivesToBeCompletedInOrder)
		{
			/* If this quest does indeed have subobjectives AND these subobjectives must be completed in order,
			assign only the first subobjective THEN add the remaining subobjectives as pending subobjectives. */
			AssignQuestInternal(FoundQuest->Subobjectives[0], QuestRowName);
			for (int32 i = 0; i < FoundQuest->Subobjectives.Num(); i++)
			{
				if (FoundQuest->Subobjectives.IsValidIndex(i + 1)) PendingSubobjectives.Add(FoundQuest->Subobjectives[i], FoundQuest->Subobjectives[i + 1]);
				else PendingSubobjectives.Add(FoundQuest->Subobjectives[i], FName(NAME_None));
			}
		}
		else
		{
			/* If this quest does indeed have subobjectives AND these subobjectives can be completed in an arbitrary order,
			recursively assign all subobjectives as new quests. */
			for (auto Subobjective : FoundQuest->Subobjectives) AssignQuestInternal(Subobjective, QuestRowName);
		}
	}

	/* Execute the questing interface. */
	ExecuteQuestingInterface(EQuestingInterfaceFunction::Began, *FoundQuest);
}

void UQuestingAssigner::CompleteQuest(const FName& QuestName, FQuest* QuestToComplete)
{
	if (QuestToComplete == nullptr) return;

	if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
	FString::Printf(TEXT("Questing Assinger: Successfully completed quest %s"), *QuestName.ToString()));

	FName ParentQuestName = QuestToComplete->ParentQuestName;

	if (!ParentQuestName.IsNone())
	{
		FQuest* ParentQuestStruct = AssignedQuests.Find(ParentQuestName);
		if (ParentQuestStruct != nullptr)
		{
			if (ParentQuestStruct->bRequiresObjectivesToBeCompletedInOrder)
			{
				FName* NextObjective = PendingSubobjectives.Find(QuestName);
				if (NextObjective != nullptr)
				{
					if (NextObjective->IsNone())
					{
						CompleteQuest(ParentQuestName, ParentQuestStruct);
						if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
						FString::Printf(TEXT("Questing Assinger: Completed all subobjectives for quest %s"), *ParentQuestName.ToString()));
					}
					else
					{
						if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
						FString::Printf(TEXT("Questing Assinger: Completed subobjective %s for quest %s. Moving onto next subobjective, %s"), *QuestName.ToString(), *ParentQuestName.ToString(), *NextObjective->ToString()));
						AssignQuestInternal(*NextObjective, ParentQuestName);
						PendingSubobjectives.Remove(QuestName);
					}
				}
			}
			else
			{
				/* Case where subobjectives can be completed in any order. */
				ParentQuestStruct->CompletedSubobjectives += 1;
				if (ParentQuestStruct->CompletedSubobjectives == ParentQuestStruct->NumSubobjectives)
				{
					CompleteQuest(ParentQuestName, ParentQuestStruct);
					if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
					FString::Printf(TEXT("Questing Assinger: Completed all subobjectives for quest %s"), *ParentQuestName.ToString()));
				}
				else
				{
					if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
					FString::Printf(TEXT("Questing Assinger: Completed subobjective %s for quest %s. %d out of %d subobjectives remaining"), *QuestName.ToString(), *ParentQuestName.ToString(), ParentQuestStruct->CompletedSubobjectives, ParentQuestStruct->NumSubobjectives));
				}
			}
		}
	}

	AssignedQuests.Remove(QuestName);
	CompletedQuests.Add(QuestName, *QuestToComplete);

	ExecuteQuestingInterface(EQuestingInterfaceFunction::Finished, *QuestToComplete);
}

void UQuestingAssigner::ExecuteQuestingInterface(EQuestingInterfaceFunction QuestingInterfaceFunction, const FQuest& QuestInfo)
{
	for (TObjectIterator<UObject> ObjectItr; ObjectItr; ++ObjectItr)
	{
		UObject* CurrentObject = *ObjectItr;
		if (CurrentObject->GetWorld() != GetWorld()) continue;

		IQuestingInterface* I = Cast<IQuestingInterface>(CurrentObject);
		if (I != nullptr)
		{
			switch (QuestingInterfaceFunction)
			{
			case EQuestingInterfaceFunction::Began:
				I->Execute_OnQuestBegan(CurrentObject, QuestInfo);
				if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
				FString::Printf(TEXT("Questing Assinger: Executing begin quest interface for quest %s with ID %d"), *QuestInfo.QuestDescription.ToString(), QuestInfo.QuestID));
				break;

			case EQuestingInterfaceFunction::Progressed:
				I->Execute_OnQuestProgressed(CurrentObject, QuestInfo);
				if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
				FString::Printf(TEXT("Questing Assinger: Executing progress quest interface for quest %s with ID %d"), *QuestInfo.QuestDescription.ToString(), QuestInfo.QuestID));
				break;

			case EQuestingInterfaceFunction::Finished:
				I->Execute_OnQuestFinished(CurrentObject, QuestInfo);
				if (bEnableDebugMessages && GEngine != nullptr) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, 
				FString::Printf(TEXT("Questing Assinger: Executing finished quest interface for quest %s with ID %d"), *QuestInfo.QuestDescription.ToString(), QuestInfo.QuestID));
				break;
			}
		}
	}
}
