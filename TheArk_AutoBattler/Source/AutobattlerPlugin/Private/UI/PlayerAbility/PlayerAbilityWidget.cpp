// Fill out your copyright notice in the Description page of Project Settings.

/* Class header. */
#include "UI/PlayerAbility/PlayerAbilityWidget.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"
#include "UI/PlayerAbility/PlayerAbilityTooltip.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* UE4 includes. */
#include "Components/Button.h"
#include "Components/Image.h"

void UPlayerAbilityWidget::BuildAbilityWidget(FAutobattlerPlayerAbilityCosmetic NewAbilityData)
{
    AbilityData = NewAbilityData;
    if (IsValid(PlayerAbilityImage)) PlayerAbilityImage->SetBrushFromTexture(NewAbilityData.Icon);

    ToolTipWidget = GetAbilityTooltip();
}

void UPlayerAbilityWidget::SetAbilityEnabled(bool NewEnabled)
{
    if (IsValid(SelectAbility)) SelectAbility->SetIsEnabled(NewEnabled);
}

void UPlayerAbilityWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (IsValid(SelectAbility)) SelectAbility->OnClicked.AddDynamic(this, &UPlayerAbilityWidget::OnSelectAbilityButtonPressed);
}

void UPlayerAbilityWidget::OnSelectAbilityButtonPressed()
{
    if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByContext(this))
    {
        ControllerComponent->RequestExecutePlayerAbility(AbilityData.ID);
    }
    else UAutobattlerFunctionLibrary::PrintErrorToLog(FString("When trying to execute player ability, local controller component is invalid!"));
}

UWidget* UPlayerAbilityWidget::GetAbilityTooltip()
{
    if (PlayerAbilityTooltipClass.Get() != nullptr)
    {
        UPlayerAbilityTooltip* PlayerAbilityTooltip = CreateWidget<UPlayerAbilityTooltip>(GetWorld(), PlayerAbilityTooltipClass);
        PlayerAbilityTooltip->BuildTooltip(AbilityData);
        return PlayerAbilityTooltip;
    }
    else UAutobattlerFunctionLibrary::PrintErrorToLog(FString("When generating ability tooltip for player ability, tooltip class is invalid!"));

    return nullptr;
}
