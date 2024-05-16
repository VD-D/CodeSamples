// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/Deployment/CharacterPanelComponents/AutobattlerDiscreteStatWidget.h"

/* Engine includes. */
#include "Components/TextBlock.h"

void UAutobattlerDiscreteStatWidget::SetBaseText(const FText& NewBaseText)
{
    BaseText = NewBaseText;
    UpdateText(BaseText);
}

void UAutobattlerDiscreteStatWidget::UpdateText(const FText& TextToUpdate)
{
    if (ensureAlwaysMsgf(IsValid(ValueText), TEXT("%s : [UpdateText] ValueText TextBlock is invalid!")))
    {
        if (TextToUpdate.EqualTo(BaseText)) ValueText->SetColorAndOpacity(BaseColor);
        else ValueText->SetColorAndOpacity(ModifiedColor);

        ValueText->SetText(TextToUpdate);
    }
}

void UAutobattlerDiscreteStatWidget::OnModifierTextUpdated_Implementation(bool IsBase)
{
    // Dummy implementation
}
