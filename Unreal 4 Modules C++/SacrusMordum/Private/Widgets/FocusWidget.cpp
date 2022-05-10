// Copyright Robert Zygmunt Uszynski 2020-2021

#include "Widgets/FocusWidget.h"

void UFocusWidget::UpdatePercentage(float NewPercentage)
{
	FocusPercentage = NewPercentage;
}

void UFocusWidget::UpdateFocusWidgetComponentVisibility(ESlateVisibility NewVisibility)
{
	if (NewVisibility != DesiredVisibility) DesiredVisibility = NewVisibility;
}

void UFocusWidget::OnFocusBegan_Implementation()
{
	/* Dummy declaration. */
}

void UFocusWidget::OnFocusInterrupted_Implementation()
{
	/* Dummy declaration. */
}

void UFocusWidget::OnFocusCompleted_Implementation()
{
	/* Dummy declaration. */
}
