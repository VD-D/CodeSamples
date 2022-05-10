// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structures/QuestingStructures.h"
#include "QuestingAssigner.generated.h"

enum class EQuestingInterfaceFunction : uint8
{
	Began,
	Progressed,
	Finished
};

struct FParentQuestIdentifier
{
	FName ParentQuest;
	FName NextSubobjective;

	FParentQuestIdentifier(FName NewParentQuest, FName NewSubobjective)
	{
		ParentQuest = NewParentQuest;
		NextSubobjective = NewSubobjective;
	}
};

UCLASS( ClassGroup=(Questing), meta=(BlueprintSpawnableComponent) )
class QUESTING_API UQuestingAssigner : public UActorComponent
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* Datatable from which quests can be created. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Questing Assigner")
	class UDataTable* QuestsDatatable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Questing Assigner")
	bool bEnableDebugMessages;

private:
	/* Map containing currently assigned quests. */
	TMap<FName, FQuest> AssignedQuests;

	/* Subobjectives which are not yet assigned as quests and are waiting on another subobjective to complete. */
	TMap<FName, FName> PendingSubobjectives;

	/* Map of completed quests. */
	TMap<FName, FQuest> CompletedQuests;

/* --- FUNCTION --- */
public:	
	/**
	 * Constructor. 
	 */
	UQuestingAssigner();

	/**
	 * Assigns a new quest from QuestsDatatable.
	 * @param QuestRowName Name of row from QuestsDatatable.
	 */
	UFUNCTION(BlueprintCallable, Category = "Questing Assigner")
	void AssignNewQuest(const FName& QuestRowName);

	/**
	 * Progresses an assigned quest. This may complete the quest if it is progressed to the end.
	 * @param AssignedQuestName The quest name to progress.
	 * @param NumStagesToProgress How many stages the quest should be progressed. For example, if the player picks up 2 of 4 required items, this value should be 2.
	 */
	UFUNCTION(BlueprintCallable, Category = "Questing Assigner")
	void ProgressQuest(const FName& AssignedQuestName, uint8 NumStagesToProgress = 1);

	UFUNCTION(BlueprintCallable, Category = "Questing Assigner")
	const TMap<FName, FQuest>& GetAssignedQuests();

	UFUNCTION(BlueprintCallable, Category = "Questing Assigner")
	const TMap<FName, FName>& GetPendingSubobjectives();

	UFUNCTION(BlueprintCallable, Category = "Questing Assigner")
	const TMap<FName, FQuest>& GetCompletedQuests();

private:
	/**
	 * Interal method of assigning quests that are appropriately tagged with a parent quest.
	 * @param QuestName Name of the quest to assign.
	 * @param ParentQuest If this quest is a subobjective, this will be its parent quest.
	 */
	void AssignQuestInternal(const FName& QuestRowName, const FName& ParentQuest = FName(NAME_None));
	
	/**
	 * Completes a quest.
	 * @param QuestName Name of the quest.
	 * @param QuestToComplete The quest that should be completed. 
	 */
	void CompleteQuest(const FName& QuestName, FQuest* QuestToComplete);

	/**
	 * Utility function for executing the questing interface.
	 * @param QuestingInterfaceFunction The function to execute.
	 * @param QuestInfo The passed quest info.
	 */
	void ExecuteQuestingInterface(EQuestingInterfaceFunction QuestingInterfaceFunction, const FQuest& QuestInfo);
};
