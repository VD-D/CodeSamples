// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/Deployment/CharacterPanelComponents/ModifierIcon.h"

/* Autobattler includes. */
#include "UI/Deployment/CharacterPanelComponents/ModifierText.h"

/* Engine includes. */
#include "Components/Image.h"
#include "Components/ProgressBar.h"

UWidget* UModifierIcon::MakeModifierIconTooltip()
{
    if (ensureAlwaysMsgf(ModifierTextWidgetClass.Get() != nullptr, TEXT("%s : [MakeModifierIconTooltip] ModifierTextWidget tooltip class is invalid!"), *GetName()))
    {
        UModifierText* ModifierTextTooltip = CreateWidget<UModifierText>(GetWorld(), ModifierTextWidgetClass);
        ModifierTextTooltip->UpdateModifierDescription(ModifierText);

        return ModifierTextTooltip;
    }

    return nullptr;
}

void UModifierIcon::BuildModifierIcon(UTexture2D* IconImage, const FText& ModifierDescription, float Duration)
{
    if (ensureAlwaysMsgf(IsValid(ModifierIcon), TEXT("%s : [BuildModifierIcon] ModifierIcon Widget Image component is invalid!")))
    {
        ModifierIcon->SetBrushFromTexture(IconImage);
    }

    ModifierText = ModifierDescription;
    ToolTipWidget = MakeModifierIconTooltip();

    if (Duration > 0.1f)
    {
        CurrentDuration = Duration;
        MaxDuration = Duration;

        FTimerDelegate Delegate;
        Delegate.BindUObject(this, &UModifierIcon::OnModifierTick);

        GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, ModifierIconTickRate, true);
    }
}

void UModifierIcon::OnModifierTick()
{
    if (!Handle.IsValid())
    {
        RemoveFromParent();
        return;
    }

    CurrentDuration -= ModifierIconTickRate;
    if (IsValid(ModifierDurationCounter))
    {
        ModifierDurationCounter->SetPercent(FMath::Max(1.0f - (CurrentDuration / MaxDuration), 0.0f));
    }

    if (CurrentDuration <= 0.0f) 
    {
        GetWorld()->GetTimerManager().ClearTimer(Handle);
        RemoveFromParent();
    }
}
