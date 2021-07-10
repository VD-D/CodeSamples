// Copyright Robert Uszynski 2021

#include "Widgets/DialogueResponse.h"
#include "Widgets/DialogueBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UDialogueResponse::NativeConstruct()
{
    if (ResponseButton != nullptr) ResponseButton->OnClicked.AddDynamic(this, &UDialogueResponse::ResponseButtonClicked);
}

void UDialogueResponse::InitialiseResponse(UDialogueBox* OwningDialogueBox, const FName& ResponseDatatableRowName, const FString& ResponseTextContent)
{
    if (OwningDialogueBox == nullptr) return;

    OwnerDialogueBox = OwningDialogueBox;
    RowNameResponse = ResponseDatatableRowName;
    ResponseText->SetText(FText::FromString(ResponseTextContent));
}

void UDialogueResponse::ResponseButtonClicked()
{
    if (OwnerDialogueBox != nullptr) OwnerDialogueBox->ProgressDialogue(RowNameResponse);
}
