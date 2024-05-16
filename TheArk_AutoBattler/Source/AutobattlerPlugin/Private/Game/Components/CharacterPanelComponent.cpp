// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Components/CharacterPanelComponent.h"

/* Autobattler includes. */
#include "DataAssets/AutobattlerConfiguration.h"
#include "DataAssets/DefaultAttack.h"
#include "UI/Deployment/CharacterPanel.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Net/UnrealNetwork.h"

UCharacterPanelComponent::UCharacterPanelComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCharacterPanelComponent::BuildFromCharacterDefinition(TSubclassOf<UCharacterPanel> CharacterPanelClass, const FAutobattlerCharacterDefinition& NewDefinition)
{
    if (IsValid(GetOwner()) && GetOwner()->HasAuthority())
    {
        const UAutobattlerConfiguration* Configuration = UAutobattlerConfiguration::GetConfigurationAsset(this);
        if (ensureAlwaysMsgf(Configuration != nullptr, TEXT("%s : [BuildFromCharacterDefinition] Failed to get configuration asset! (was this function called before game was initialised?)") ))
        {   
            FCharacterBaseStats NewBaseStats;
            NewBaseStats.BaseHealth = NewDefinition.BaseHealth;
            NewBaseStats.BaseMovementSpeed = NewDefinition.MovementSpeed;
            NewBaseStats.BaseCriticalChance = NewDefinition.CriticalChance;
            NewBaseStats.BaseCriticalMultiplier = NewDefinition.CriticalMultiplier;

            const FText* FoundResistance = Configuration->ResistanceTextMap.Find(NewDefinition.BaseResistanceType);
            NewBaseStats.BaseResistanceType = FoundResistance != nullptr ? *FoundResistance : FText::GetEmpty();

            const FText* FoundDamageType = Configuration->DamageTextMap.Find(NewDefinition.BaseDamageType);
            NewBaseStats.BaseDamageType = FoundDamageType != nullptr ? *FoundDamageType : FText::GetEmpty();

            BuildFromDefinitionInternal(CharacterPanelClass, NewDefinition.CharacterIcon, NewDefinition.CharacterName, NewBaseStats, NewDefinition.AttackImplementation, NewDefinition.AbilityImplementation);
        }
    }
}

void UCharacterPanelComponent::BuildFromDefinitionInternal_Implementation(TSubclassOf<UCharacterPanel> CharacterPanelClass, UTexture2D* NewIcon, const FText& NewName, const FCharacterBaseStats& NewBaseStats, const UAutobattlerAttack* NewAttackDefinition, const UAutobattlerAbility* NewAbilityDefinition)
{
    if (ensureAlwaysMsgf(CharacterPanelClass.Get() != nullptr, TEXT("%s : [BuildFromDefinitionInternal] Character Panel Widget Class is invalid! (Did not call SetCharacterPanelWidgetClass?)"), *GetName()))
    {
        if (IsValid(CharacterPanel)) CharacterPanel->RemoveFromViewport();

        CharacterPanel = CreateWidget<UCharacterPanel>(GetWorld(), CharacterPanelClass);
        CharacterPanel->BuildCharacterPanel(
            NewIcon, 
            NewName, 
            NewBaseStats, 
            NewAttackDefinition, 
            NewAbilityDefinition
        );
        CharacterPanel->AddToViewport();
        CharacterPanel->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UCharacterPanelComponent::HideActiveCharacterPanel(const UObject* WorldContextObject)
{
    if (GEngine == nullptr) return;

    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        for (TObjectIterator<UCharacterPanelComponent> PanelItr; PanelItr; ++PanelItr)
        {
            UCharacterPanelComponent* LiveCharacterPanelComponent = *PanelItr;
            if (LiveCharacterPanelComponent == nullptr || LiveCharacterPanelComponent->GetWorld() != World) continue;

            LiveCharacterPanelComponent->ShowCharacterPanel(false);
        }
    }
}

bool UCharacterPanelComponent::ShowCharacterPanel(bool NewShow)
{
	if (IsLocked) return false;

	for (TObjectIterator<UCharacterPanel> PanelItr; PanelItr; ++PanelItr)
	{
		UCharacterPanel* LiveCharacterPanel = *PanelItr;
		if (LiveCharacterPanel == nullptr 
			|| LiveCharacterPanel->GetWorld() != GetWorld()
			|| LiveCharacterPanel == CharacterPanel
		) continue;

		if (LiveCharacterPanel->IsVisible()) 
        {
            return false; // NOTE: If any other character panel is visible, we will not show another one.
        }
	}

	if (!NewShow && CharacterPanel == nullptr) 
    {
        return false;
    }
	if (!NewShow && CharacterPanel != nullptr)
	{
		CharacterPanel->SetVisibility(ESlateVisibility::Collapsed);
		return true;
	}

	if (NewShow && CharacterPanel != nullptr)
	{
		CharacterPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return true;
	}

	return false;
}

void UCharacterPanelComponent::LockCharacterPanel(bool NewLock)
{
	if (NewLock)
	{
		if (ShowCharacterPanel(true)) IsLocked = true;
	}
	else 
	{
		if (CharacterPanel != nullptr)
		{
			CharacterPanel->SetVisibility(ESlateVisibility::Collapsed);
			IsLocked = false;
		}
	}
}

void UCharacterPanelComponent::AddModifier_Implementation(UTexture2D* IconImage, const FText& ModifierDescription, float Duration)
{
    if (IsValid(CharacterPanel))
    {
        CharacterPanel->AddModifier(IconImage, ModifierDescription, Duration);
    }
}

void UCharacterPanelComponent::UpdateStatNumeric_Implementation(EAutobattlerStatType StatTypeToModify, float NewValue, bool IsBase)
{
    if (IsValid(CharacterPanel))
    {
        CharacterPanel->UpdateStat(StatTypeToModify, NewValue, IsBase);
    }
}

void UCharacterPanelComponent::UpdateStatDiscrete_Implementation(EAutobattlerDiscreteType StatTypeToModify, const FText& NewValue, bool IsBase)
{
    if (IsValid(CharacterPanel))
    {
        CharacterPanel->UpdateStat(StatTypeToModify, NewValue, IsBase);
    }
}
