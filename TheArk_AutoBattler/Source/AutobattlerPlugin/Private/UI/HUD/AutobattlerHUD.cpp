// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/HUD/AutobattlerHUD.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"
#include "Core/AutobattlerManager.h"
#include "Game/Grid/AutobattlerGrid.h"
#include "UI/CharacterSelect/CharacterSelectContainerWidget.h"

/* Engine includes. */
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UAutobattlerHUD::SetCurrentBudget(int32 NewCurrentBudget)
{
    SetIntAsText(CurrentBudget, NewCurrentBudget);
}

void UAutobattlerHUD::SetMaxBudget(int32 NewMaxBudget)
{
    SetIntAsText(MaxBudget, NewMaxBudget);
}

void UAutobattlerHUD::SetPendingBudget(int32 NewPendingBudget)
{
    SetIntAsText(PendingBudget, NewPendingBudget);
}

void UAutobattlerHUD::SetPendingBudgetVisibility(bool ShouldBeVisible)
{
    if (IsValid(PendingBudget))
    {
        PendingBudget->SetVisibility(ShouldBeVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
    }

    OnPendingBudgetVisibilityChanged(ShouldBeVisible);
}

void UAutobattlerHUD::SetAutobattlerGamePhaseText(EAutobattlerPhase PhaseToSet)
{
    if (IsValid(CurrentGamePhase))
    {
        if (FText* Text = PhaseToTextMap.Find(PhaseToSet)) CurrentGamePhase->SetText(*Text);
        else CurrentGamePhase->SetText(FText::GetEmpty());
    }
}

void UAutobattlerHUD::SetDeploymentEnabled(bool IsEnabled)
{
    if (UCharacterSelectContainerWidget* Container = GetCharacterSelectContainer())
    {
        Container->SetEnableDeployment(IsEnabled);
    }
}

void UAutobattlerHUD::OnPlacementReasonChange_Implementation(EPlaceReason PlacementReason)
{
    // Dummy implementation.
}

void UAutobattlerHUD::NativeConstruct()
{
    Super::NativeConstruct();
    if (IsValid(ToggleAvailableDeploymentFields) 
        && !ToggleAvailableDeploymentFields->OnClicked.IsAlreadyBound(this, &UAutobattlerHUD::OnToggleAvailableDeploymentFieldsClicked)) 
    ToggleAvailableDeploymentFields->OnClicked.AddDynamic(this, &UAutobattlerHUD::OnToggleAvailableDeploymentFieldsClicked);
}

void UAutobattlerHUD::OnPendingBudgetVisibilityChanged_Implementation(bool NewShouldBeVisible)
{
    // Dummy implementation.
}

void UAutobattlerHUD::SetIntAsText(UTextBlock* TextBlock, int32 Value)
{
    if (IsValid(TextBlock))
    {
        TextBlock->SetText(FText::AsNumber(Value));
    }
}

void UAutobattlerHUD::OnToggleAvailableDeploymentFieldsClicked()
{
    if (APlayerController* LocalPlayerController = UAutobattlerControllerComponent::GetLocalPlayerController(this))
    {
        AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this);
        AAutobattlerGrid* Grid = IsValid(Manager) ? Manager->GetAutobattlerGrid() : nullptr;
        UAutobattlerControllerComponent* ControllerComponent = Cast<UAutobattlerControllerComponent>(LocalPlayerController->GetComponentByClass(UAutobattlerControllerComponent::StaticClass()));

        if (IsValid(Grid) && IsValid(ControllerComponent))
        {
            Manager->RequestChangeGridVisualisationForEntity(ControllerComponent->GetIdentity(), Grid->GetIsCurrentlyHidden());
        }
    }
}
