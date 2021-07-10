// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Structures/DialogueStructures.h"
#include "TopDownDialogueInterface.generated.h"

UINTERFACE(MinimalAPI)
class UTopDownDialogueInterface : public UInterface
{
	GENERATED_BODY()
};

/* Interface called by the dialogue room on listening objects which respond to dialogue updates. */
class TOPDOWNDIALOGUES_API ITopDownDialogueInterface
{
	GENERATED_BODY()

public:
	/**
	 * Called whenever a dialogue begins. Since dialogues are not indexed, this is called at the beginning of any
	 * arbitrary dialogue. 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Top Down Dialogue Interface")
	void OnDialogueBegan();

	/**
	 * Called whenever a dialogue is updated (this includes when it begins). Passess the properties of the current line
	 * of dialogue.
	 * @param UpdatedProperties The properties of the current line of dialogue.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Top Down Dialogue Interface")
	void OnDialogueUpdated(const FTopDownDialogueProperties& UpdatedProperties);

	/**
	 * Called whenever a dialogue end. Since dialogues are not indexed, this is called at the end of any
	 * arbitrary dialogue.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Top Down Dialogue Interface")
	void OnDialogueEnded();
};
