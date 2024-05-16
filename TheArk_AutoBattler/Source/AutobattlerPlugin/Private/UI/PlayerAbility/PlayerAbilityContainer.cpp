// Fill out your copyright notice in the Description page of Project Settings.

/* Class header. */
#include "UI/PlayerAbility/PlayerAbilityContainer.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"
#include "UI/PlayerAbility/PlayerAbilityWidget.h"

void UPlayerAbilityContainer::SetPlayerAbilitiesEnabled(bool NewEnabled)
{
    AbilitiesEnabledState = NewEnabled;

    for (auto PlayerAbilityWidget : PlayerAbilityWidgets)
    {
        PlayerAbilityWidget->SetAbilityEnabled(AbilitiesEnabledState);
    }
}

void UPlayerAbilityContainer::UpdatePlayerAbilities()
{
    for (auto PlayerAbilityWidget : PlayerAbilityWidgets)
    {
        PlayerAbilityWidget->RemoveFromParent();
    }

    if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByContext(this))
    {
        TArray<FAutobattlerPlayerAbilityCosmetic> PlayerAbilityProps;
        ControllerComponent->GetCosmeticPlayerAbilityData(PlayerAbilityProps);
        PlayerAbilityWidgets.Empty(PlayerAbilityProps.Num());

        for (auto PlayerAbilityProp : PlayerAbilityProps)
        {
            UPlayerAbilityWidget* NewAbilityWidget = CreateWidget<UPlayerAbilityWidget>(GetWorld(), PlayerAbilityWidgetClass);
            NewAbilityWidget->BuildAbilityWidget(PlayerAbilityProp);
            NewAbilityWidget->SetAbilityEnabled(AbilitiesEnabledState);
            PlayerAbilityWidgets.Emplace(NewAbilityWidget);
        }
    }
    else PlayerAbilityWidgets.Empty();
}
