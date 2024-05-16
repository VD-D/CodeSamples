// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/Deployment/CharacterPanelComponents/ModifierText.h"

/* Engine includes. */
#include "Components/RichTextBlock.h"

void UModifierText::UpdateModifierDescription(const FText& NewRichTextDescription)
{
    if (ensureAlwaysMsgf(IsValid(DescriptionText), TEXT("%s : [UpdateModifierDescription] DescriptionText RichTextBlock is invalid!")))
    {
        DescriptionText->SetText(NewRichTextDescription);
    }
}
