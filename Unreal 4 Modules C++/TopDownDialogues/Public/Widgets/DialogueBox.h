// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueBox.generated.h"

/* The containing widget which shows all information about the dialogue. */
UCLASS()
class TOPDOWNDIALOGUES_API UDialogueBox : public UUserWidget
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* Responses will be added to this scroll box. */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* ResponsesBox;

	/* The speaker's name will be displayed in this widget. */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SpeakerName;

	/* The actual dialogue content will be displayed in this widget. */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* DialogueContent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Box")
	FString DefaultEndConversationResponse = FString("End");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Box")
	TSubclassOf<class UDialogueResponse> DialogueResponseClass;

private:
	/* Mark that ADialogueRoom is a friend class of this widget. */
	friend class ADialogueRoom;

	/* The dialogue room which "owns" this widget. */
	class ADialogueRoom* OwningDialogueRoom;

	/* Array of current responses. */
	TArray<class UDialogueResponse*> Responses;

	/* Whether or not the current line of dialogue is the last. */
	bool bIsMarkedAsEnd;

/* --- FUNCTIONS --- */
public:
	void UpdateDialogue(const FString& NewSpeakerName, const FString& NewDialogueContent, bool bIsLastDialogueLine, const TMap<FName, FString>& NewResponses);

	void ProgressDialogue(const FName& ResponseRowName);

private:
	void CreateResponse(const FName& ResponseRowName, const FString& ResponseText);
};
