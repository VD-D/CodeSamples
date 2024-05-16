// Copyright Juggler Games 2024
// Contributors: Robert Uszynski

/* Class header. */
#include "UI/Debug/CharacterDebugWidget.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"

/* Engine includes. */
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "DrawDebugHelpers.h"

void UCharacterDebugWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (IsValid(VisualiseTargetCheckbox)) VisualiseTargetCheckbox->OnCheckStateChanged.AddDynamic(this, &UCharacterDebugWidget::OnVisualiseTargetCheckboxStateChange);
    if (IsValid(VisualiseMoveToLocationCheckbox)) VisualiseMoveToLocationCheckbox->OnCheckStateChanged.AddDynamic(this, &UCharacterDebugWidget::OnVisualiseMoveToLocationCheckboxStateChange);
}

void UCharacterDebugWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (EnableUpdate)
    {
        if (IsValid(CurrentDebugController) && CurrentDebugController->HasAuthority())
        {
            if (VisualiseCurrentTarget) DrawLineToTarget();
            if (VisualiseMoveToPoint) DrawLineToMoveToPoint();
            UpdateTextWidgets();
        }
        else
        {
            EnableUpdate = false;
            if (IsValid(CurrentActionText)) CurrentActionText->SetText(FText::GetEmpty());
            if (IsValid(IDText)) IDText->SetText(FText::GetEmpty());
            if (IsValid(OwnerText)) OwnerText->SetText(FText::GetEmpty());
            if (IsValid(CurrentTargetingTypeText)) CurrentTargetingTypeText->SetText(FText::GetEmpty());
        }
    }
}

void UCharacterDebugWidget::DrawLineToTarget()
{
    FAbilityTargetingProperties AbilityTargetingProps;
    CurrentDebugController->GetCurrentTargetingProperties(AbilityTargetingProps);

    if (AbilityTargetingProps.TargetingMode == ESkillTargetingMode::None) return;
    if (AbilityTargetingProps.TargetingMode == ESkillTargetingMode::Actor && !IsValid(AbilityTargetingProps.TargetCharacter)) return;

    const FVector Target = AbilityTargetingProps.TargetingMode == ESkillTargetingMode::Actor ? AbilityTargetingProps.TargetCharacter->GetActorLocation() : AbilityTargetingProps.TargetLocation;
    DrawDebugLine(
        GetWorld(),
        CurrentDebugController->GetPawn()->GetActorLocation(),
        Target,
        FColor::Red,
        false,
        0.01f,
        0,
        5.0f
    );

    DrawDebugSphere(
        GetWorld(),
        Target,
        32.0f,
        12,
        FColor::Red,
        false,
        0.01f
    );
}

void UCharacterDebugWidget::DrawLineToMoveToPoint()
{
    DrawDebugLine(
        GetWorld(),
        CurrentDebugController->GetPawn()->GetActorLocation(),
        CurrentDebugController->GetDesiredMoveToLocation(),
        FColor::Cyan,
        false,
        0.01f,
        0,
        5.0f
    );

    DrawDebugSphere(
        GetWorld(),
        CurrentDebugController->GetDesiredMoveToLocation(),
        32.0f,
        12,
        FColor::Cyan,
        false,
        0.01f
    );
}

void UCharacterDebugWidget::UpdateTextWidgets()
{
    AAutobattlerCharacter* DebugCharacter = Cast<AAutobattlerCharacter>(CurrentDebugController->GetPawn());

    if (IsValid(IDText) && IsValid(DebugCharacter))
    {
        IDText->SetText(FText::AsNumber(DebugCharacter->GetID()));
    }

    if (IsValid(OwnerText) && IsValid(DebugCharacter))
    {
        if (FText* Text = OwnerToTextMap.Find(DebugCharacter->GetOwnerIdentity())) OwnerText->SetText(*Text);
        else OwnerText->SetText(FText::GetEmpty());
    }

    if (IsValid(CurrentActionText) && IsValid(DebugCharacter))
    {
        if (FText* Text = ActionToTextMap.Find(DebugCharacter->GetCurrentAction())) CurrentActionText->SetText(*Text);
        else CurrentActionText->SetText(FText::GetEmpty());
    }

    if (IsValid(CurrentTargetingTypeText))
    {
        FAbilityTargetingProperties AbilityTargetingProps;
        CurrentDebugController->GetCurrentTargetingProperties(AbilityTargetingProps);

        if (FText* Text = TargetingToTextMap.Find(AbilityTargetingProps.TargetingMode)) CurrentTargetingTypeText->SetText(*Text);
        else CurrentTargetingTypeText->SetText(FText::GetEmpty());
    }
}

void UCharacterDebugWidget::SetNewDebugController(AAutobattlerAIController* NewController)
{
    if (!IsValid(NewController)) return;

    if (IsValid(CurrentDebugController)) CurrentDebugController->SetDebugEnabled(false);

    CurrentDebugController = NewController;
    CurrentDebugController->SetDebugEnabled(true);
    VisualiseCurrentTarget = true;
    VisualiseMoveToPoint = true;
    EnableUpdate = true;

    if (IsValid(VisualiseTargetCheckbox)) VisualiseTargetCheckbox->SetIsChecked(true);
    if (IsValid(VisualiseMoveToLocationCheckbox)) VisualiseMoveToLocationCheckbox->SetIsChecked(true);
}

void UCharacterDebugWidget::OnVisualiseTargetCheckboxStateChange(bool NewState)
{
    VisualiseCurrentTarget = NewState;
}

void UCharacterDebugWidget::OnVisualiseMoveToLocationCheckboxStateChange(bool NewState)
{
    VisualiseMoveToPoint = NewState;
}
