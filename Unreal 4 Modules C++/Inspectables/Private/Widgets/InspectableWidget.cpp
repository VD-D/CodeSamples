// Copyright Robert Zygmunt Uszynski 2021-2022


#include "Widgets/InspectableWidget.h"

#include "Components/Button.h"
#include "Components/RichTextBlock.h"

void UInspectableWidget::CloseButtonPressed_Implementation()
{
    RemoveFromViewport();
}

void UInspectableWidget::UpdateText(const FText& NewText)
{
    if (TextBlock != nullptr) TextBlock->SetText(NewText);
}

void UInspectableWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseButton != nullptr) 
    {
        CloseButton->OnClicked.AddDynamic(this, &UInspectableWidget::BroadcastClosedDelegate);
        CloseButton->OnClicked.AddDynamic(this, &UInspectableWidget::CloseButtonPressed);
    }
}

void UInspectableWidget::BroadcastClosedDelegate()
{
    InspectableWidgetCloseButtonClicked.Broadcast();
}