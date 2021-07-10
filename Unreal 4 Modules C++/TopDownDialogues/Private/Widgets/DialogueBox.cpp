// Copyright Robert Uszynski 2021

#include "Widgets/DialogueBox.h"
#include "Widgets/DialogueResponse.h"
#include "Actors/DialogueRoom.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"

void UDialogueBox::UpdateDialogue(const FString& NewSpeakerName, const FString& NewDialogueContent, bool bIsLastDialogueLine, const TMap<FName, FString>& NewResponses)
{
    if (SpeakerName != nullptr) SpeakerName->SetText(FText::FromString(NewSpeakerName));
    if (DialogueContent != nullptr) DialogueContent->SetText(FText::FromString(NewDialogueContent));

    for (auto Response : Responses) Response->RemoveFromParent();
    Responses.Empty();

    if (NewResponses.Num() != 0)
    {
        for (auto NewResponse : NewResponses) CreateResponse(NewResponse.Key, NewResponse.Value);

        bIsMarkedAsEnd = bIsLastDialogueLine;
    }
    else
    {
        CreateResponse(FName("None"), DefaultEndConversationResponse);
        bIsMarkedAsEnd = true;
    }
}

void UDialogueBox::ProgressDialogue(const FName& ResponseRowName)
{
    if (bIsMarkedAsEnd)
    {
        if (OwningDialogueRoom != nullptr) 
        {
            OwningDialogueRoom->EndDialogue();
            for (auto Response : Responses) Response->RemoveFromParent();
            Responses.Empty();
        }
    }
    else
    {
        if (OwningDialogueRoom != nullptr) OwningDialogueRoom->UpdateDialogue(ResponseRowName);
    }
}

void UDialogueBox::CreateResponse(const FName& ResponseRowName, const FString& ResponseText)
{
    if (ResponsesBox == nullptr) return;

    UDialogueResponse* DialogueResponse = CreateWidget<UDialogueResponse>(GetWorld(), DialogueResponseClass);
    if (DialogueResponse == nullptr) return;

    DialogueResponse->InitialiseResponse(this, ResponseRowName, ResponseText);
    ResponsesBox->AddChild(DialogueResponse);
    Responses.Add(DialogueResponse);
}