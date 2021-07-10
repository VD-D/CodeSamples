// Copyright Robert Uszynski 2021

#include "Actors/DialogueRoom.h"
#include "Engine/DataTable.h"
#include "Interfaces/TopDownDialogueInterface.h"
#include "Structures/DialogueStructures.h"
#include "Widgets/DialogueBox.h"

ADialogueRoom::ADialogueRoom()
{
	PrimaryActorTick.bCanEverTick = false;
	DialogueBoxZOrder = 1;
}

void ADialogueRoom::BeginDialogue(const FName& FirstDialogueRowName)
{
	UpdateDialogue(FirstDialogueRowName);
	ExecuteDialogueInterface(EDialogueInterfaceFunction::Start, FTopDownDialogueProperties());
}

void ADialogueRoom::UpdateDialogue(const FName& NextDialogueRowName)
{
	if (DialogueDatatable == nullptr) return;
	
	FTopDownDialogueProperties* Properties = DialogueDatatable->FindRow<FTopDownDialogueProperties>(NextDialogueRowName, FString("Dialogue Properties"));
	if (Properties == nullptr) return;

	if (SpawnedDialogueBox == nullptr)
	{
		SpawnedDialogueBox = CreateWidget<UDialogueBox>(GetWorld(), DialogeBoxWidgetClass);
		if (SpawnedDialogueBox != nullptr)
		{
			SpawnedDialogueBox->UpdateDialogue(Properties->SpeakerName, Properties->DialogueText, Properties->bIsLastLineOfConversation, Properties->BranchToMap);
			SpawnedDialogueBox->OwningDialogueRoom = this;
			SpawnedDialogueBox->AddToViewport(DialogueBoxZOrder);
		}
	}
	else
	{
		SpawnedDialogueBox->UpdateDialogue(Properties->SpeakerName, Properties->DialogueText, Properties->bIsLastLineOfConversation, Properties->BranchToMap);
		SpawnedDialogueBox->SetVisibility(ESlateVisibility::Visible);
	}

	ExecuteDialogueInterface(EDialogueInterfaceFunction::End, *Properties);
}

void ADialogueRoom::EndDialogue()
{
	if (SpawnedDialogueBox != nullptr) SpawnedDialogueBox->SetVisibility(ESlateVisibility::Collapsed);
	ExecuteDialogueInterface(EDialogueInterfaceFunction::End, FTopDownDialogueProperties());
}

void ADialogueRoom::ExecuteDialogueInterface(EDialogueInterfaceFunction FunctionToExecute, const FTopDownDialogueProperties& TopDownDialogueProperties)
{
	for (TObjectIterator<UObject> ObjectItr; ObjectItr; ++ObjectItr)
	{
		UObject* CurrentObject = *ObjectItr;
		if (CurrentObject->GetWorld() != GetWorld()) continue;

		ITopDownDialogueInterface* I = Cast<ITopDownDialogueInterface>(CurrentObject);
		if (I == nullptr) continue;

		switch (FunctionToExecute)
		{
			case EDialogueInterfaceFunction::Start:
				I->Execute_OnDialogueBegan(CurrentObject);
				break;

			case EDialogueInterfaceFunction::Update:
				I->Execute_OnDialogueUpdated(CurrentObject, TopDownDialogueProperties);
				break;

			case EDialogueInterfaceFunction::End:
				I->Execute_OnDialogueEnded(CurrentObject);
				break;
		}
	}
}
