// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/HealthBar/HealthBar.h"

/* Engine includes. */
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBar::UpdateHealthValues(float NewCurrentHealth, float NewMaxHealth)
{
    float ClampedCurrentHealth = FMath::RoundToFloat(FMath::Clamp(NewCurrentHealth, 0.0f, NewMaxHealth));
    float ClampedMaxHealth = FMath::RoundToFloat(FMath::Max(NewMaxHealth, 0.01f));
    if (IsValid(CurrentHealth)) CurrentHealth->SetText(FText::AsNumber(ClampedCurrentHealth));
    if (IsValid(MaxHealth)) MaxHealth->SetText(FText::AsNumber(ClampedMaxHealth));
    if (IsValid(HealthPercent)) HealthPercent->SetPercent(ClampedCurrentHealth / ClampedMaxHealth);
}
