// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/Deployment/CharacterPanelComponents/AutobattlerStatWidget.h"

/* Engine includes. */
#include "Components/TextBlock.h"

void UAutobattlerStatWidget::UpdateValue(bool IsBase, float ModifierScale)
{
    if (IsBase)
    {
        if (ensureAlwaysMsgf(IsValid(StatBaseValue), TEXT("%s : [UpdateValue] StatBaseValue TextBlock is invalid!"), *GetName()))
        {
            StatBaseValue->SetText(FText::AsNumber(ModifierScale));
        }
    }
    else
    {
        if (ensureAlwaysMsgf(IsValid(StatModifierValue), TEXT("%s : [UpdateValue] StatModifierValue TextBlock is invalid!"), *GetName()))
        {
            CurrentModifierValue += ModifierScale;
            CurrentModifierValue = FMath::RoundToFloat(CurrentModifierValue);
            StatModifierValue->SetText(FText::AsNumber(CurrentModifierValue));
            if (CurrentModifierValue == 0.0f) StatModifierValue->SetColorAndOpacity(ZeroColor);
            else if (CurrentModifierValue > 0.0f) StatModifierValue->SetColorAndOpacity(PositiveColor);
            else StatModifierValue->SetColorAndOpacity(NegativeColor);
            OnStatModifierChange(CurrentModifierValue);
        }
    }
}

void UAutobattlerStatWidget::OnStatModifierChange_Implementation(float NewStatValue)
{
    // Dummy implementation
}
