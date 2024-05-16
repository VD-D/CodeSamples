// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/CharacterSelect/CharacterSelectWidget.h"

/* Autobattler includes. */
#include "Core/AutobattlerManager.h"
#include "Core/AutobattlerControllerComponent.h"
#include "DataAssets/AutobattlerConfiguration.h"
#include "Game/Components/CharacterPanelComponent.h"
#include "Game/Misc/CharacterPanelActor.h"
#include "UI/Deployment/CharacterPanel.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCharacterSelectWidget::BuildCharacterSelectWidget(int32 NewRepresentedCharacterID, EEntity WhoOwns, const FCharacterListing& NewCharacterListing)
{
    RepresentedCharacterID = NewRepresentedCharacterID;
    InternalListing = NewCharacterListing;
    WhoOwnsInternal = WhoOwns;

    if (const UDataTable* CharactersDT = GetCharactersDatatableFromManager())
    {
        if (FAutobattlerCharacterDefinition* Definition = CharactersDT->FindRow<FAutobattlerCharacterDefinition>(NewCharacterListing.CharacterRowName, FString("Character Definition")))
        {
            if (IsValid(CharacterName)) CharacterName->SetText(Definition->CharacterName);
            if (IsValid(CharacterImage)) CharacterImage->SetBrushFromTexture(Definition->CharacterIcon);
            if (IsValid(CharacterCost)) CharacterCost->SetText(FText::AsNumber(Definition->BudgetCost));
        }
        else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("When constructing character select widget, row called %s does not exist!"), *NewCharacterListing.CharacterRowName.ToString()));
    }
}

void UCharacterSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (IsValid(DeployButton))
    {
        DeployButton->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnDeployButtonClicked);
        DeployButton->OnHovered.AddDynamic(this, &UCharacterSelectWidget::OnDeployButtonMouseOverBegin);
    }
}

void UCharacterSelectWidget::SetDeployButtonEnabled(bool Enable)
{
    if (IsValid(DeployButton))
    {
        DeployButton->SetIsEnabled(Enable);
    }
}

void UCharacterSelectWidget::OnDeployButtonClicked()
{
    if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByID(this, WhoOwnsInternal))
    {
        ControllerComponent->RPC_BeginFloatingCharacterForPlayer(WhoOwnsInternal, RepresentedCharacterID);
    }

    if (ACharacterPanelActor* CharacterPanelActor = ACharacterPanelActor::GetCharacterPanelActorByID(this, RepresentedCharacterID))
    {
        CharacterPanelActor->GetCharacterPanel()->ShowCharacterPanel(false);
    }
}

void UCharacterSelectWidget::OnDeployButtonMouseOverBegin()
{
    UCharacterPanelComponent::HideActiveCharacterPanel(this);
    if (ACharacterPanelActor* CharacterPanelActor = ACharacterPanelActor::GetCharacterPanelActorByID(this, RepresentedCharacterID))
    {
        CharacterPanelActor->GetCharacterPanel()->ShowCharacterPanel(true);
    }
}

const UDataTable* UCharacterSelectWidget::GetCharactersDatatableFromManager() const
{
    if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
    {
        if (const UAutobattlerConfiguration* Configuration = Manager->GetAutobattlerConfigurationAsset())
        {
            if (const UDataTable* CharactersDT = Configuration->AllCharactersDataTable)
            {
                return CharactersDT;
            }
            else UAutobattlerFunctionLibrary::PrintErrorToLog(FString("When constructing character select widget, characters datatable was invalid!"));
        } 
        else UAutobattlerFunctionLibrary::PrintErrorToLog(FString("When constructing character select widget, configuration asset was invalid!"));
    }
    else UAutobattlerFunctionLibrary::PrintErrorToLog(FString("When constructing character select widget, manager was invalid!"));

    return nullptr;
}
