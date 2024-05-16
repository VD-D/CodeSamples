// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/CharacterSelect/CharacterSelectContainerWidget.h"

/* Autobattler includes. */
#include "UI/CharacterSelect/CharacterSelectWidget.h"

/* Engine includes. */
#include "Components/VerticalBox.h"

void UCharacterSelectContainerWidget::BuildCharacterSelectWidget(int32 NewRepresentedCharacterID, EEntity WhoOwns, const FCharacterListing& NewCharacterListing)
{
    if (IsValid(CharacterSelectBox) && CharacterSelectWidgetClass.Get() != nullptr)
    {
        UCharacterSelectWidget* NewCharacterSelectWidget = CreateWidget<UCharacterSelectWidget>(GetWorld(), CharacterSelectWidgetClass.Get());
        if (!IsValid(NewCharacterSelectWidget)) return;
        NewCharacterSelectWidget->BuildCharacterSelectWidget(NewRepresentedCharacterID, WhoOwns, NewCharacterListing);
        CharacterSelectBox->AddChildToVerticalBox(NewCharacterSelectWidget);
        CharacterSelectWidgets.Emplace(NewCharacterSelectWidget);
    }
}

void UCharacterSelectContainerWidget::RemoveCharacterSelectWidget(int32 IDToRemove)
{
    UCharacterSelectWidget* WidgetToRemove = nullptr;
    for (auto CharacterSelectWidget : CharacterSelectWidgets)
    {
        if (IDToRemove == CharacterSelectWidget->GetRepresentedCharacterID())
        {
            CharacterSelectWidget->RemoveFromParent();
            WidgetToRemove = CharacterSelectWidget;
            break;
        }
    }

    if (WidgetToRemove != nullptr)
    {
        CharacterSelectWidgets.RemoveSwap(WidgetToRemove, true);
    }
}

void UCharacterSelectContainerWidget::SetEnableDeployment(bool IsEnabled)
{
    for (auto CharacterSelectWidget : CharacterSelectWidgets)
    {
        CharacterSelectWidget->SetDeployButtonEnabled(IsEnabled);
    }
}
