// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "Structures/DialogueStructures.h"
#include "GameFramework/Actor.h"
#include "DialogueRoom.generated.h"

enum class EDialogueInterfaceFunction : uint8
{
	Start,
	End,
	Update
};

/* Used for creating top-down "animated" dialogues. */
UCLASS()
class TOPDOWNDIALOGUES_API ADialogueRoom : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- COMPONENTS --- */

	/* --- CONFIGURABLE --- */
	/* Datatable of TopDownDialogueProperties structure type. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Dialogues")
	class UDataTable* DialogueDatatable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Dialogues")
	TSubclassOf<class UDialogueBox> DialogeBoxWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Dialogues")
	int32 DialogueBoxZOrder;

private:
	class UDialogueBox* SpawnedDialogueBox;

public:	
	/**
	 * Constructor. 
	 */
	ADialogueRoom();

	/**
	 * Begins a dialogue.
	 * @param FirstDialogueRowName Row name of the first dialogue as it appears in DialogueDatatable. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue Room")
	void BeginDialogue(const FName& FirstDialogueRowName);

	/**
	 * Updates the dialogue from a given row.
	 * @param NextDialogueRowName Row name of the dialogue as it appears in DialogueDatatable.
	 */
	void UpdateDialogue(const FName& NextDialogueRowName);

	/**
	 * Ends the dialogue. 
	 */
	void EndDialogue();

private:
	void ExecuteDialogueInterface(EDialogueInterfaceFunction FunctionToExecute, const FTopDownDialogueProperties& TopDownDialogueProperties);

};
