// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/ChargeBar/ChargeBar.h"

/* Engine includes. */
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UChargeBar::UpdateChargeValues(int32 NewCurrentCharge, int32 NewMaxCharge)
{
    if (IsValid(CurrentCharge)) CurrentCharge->SetText(FText::AsNumber(NewCurrentCharge));
    if (IsValid(MaxCharge)) MaxCharge->SetText(FText::AsNumber(NewMaxCharge));
    if (IsValid(ChargePercent)) ChargePercent->SetPercent(NewCurrentCharge / NewMaxCharge);
}
