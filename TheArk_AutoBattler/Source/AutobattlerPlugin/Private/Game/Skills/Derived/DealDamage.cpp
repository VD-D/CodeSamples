// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Skills/Derived/DealDamage.h"

/* Autobattler includes. */
#include "DataAssets/AutobattlerConfiguration.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Utility/AutobattlerFunctionLibrary.h"

void UDealDamage::ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation)
{
    if (SkillTargetingMode != ESkillTargetingMode::Actor || !IsValid(Target)) return;

    float ResistanceModifier = 1.0f;
    float CriticalMultiplier = 1.0f;

    if (AffectedByResistance)
    {
        if (const UAutobattlerConfiguration* ConfigurationAsset = UAutobattlerConfiguration::GetConfigurationAsset(SkillOwner))
        {
            TMap<EDamageType, FResistanceContainer> DamageModifierMap = ConfigurationAsset->DamageModifierMap;
            if (FResistanceContainer* ResistanceContainer = DamageModifierMap.Find(SkillOwner->GetDamageType()))
            {
                if (float* FoundModifier = ResistanceContainer->Resistances.Find(Target->GetResistanceType()))
                {
                    ResistanceModifier = *FoundModifier;
                }
                else UAutobattlerFunctionLibrary::PrintWarningToLog(
                    FString::Printf(TEXT("When %s tried to deal damage, could not find resistance type %s defending against damage type %s (resistances not applied)!"), 
                        *SkillOwner->GetName(),
                        *UAutobattlerEnums::ResistanceTypeToString(Target->GetResistanceType()),
                        *UAutobattlerEnums::DamageTypeToString(SkillOwner->GetDamageType())
                    ));
            }
            else UAutobattlerFunctionLibrary::PrintWarningToLog(
                FString::Printf(TEXT("When %s tried to deal damage, there exists no resistance definition for damage type %s (resistances not applied)!"), 
                    *SkillOwner->GetName(),
                    *UAutobattlerEnums::DamageTypeToString(SkillOwner->GetDamageType())
                ));
        }
        else UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("When %s tried to deal damage, could not get configuration asset (resistances not applied)!"), *SkillOwner->GetName()));
    }

    if (CanBeCritical)
    {
        bool IsCritical = FMath::RandRange(0.0f, 1.0f) <= FMath::Clamp(SkillOwner->GetCriticalChance(), 0.0f, 1.0f);
        CriticalMultiplier = IsCritical ? SkillOwner->GetCriticalMultiplier() : 1.0f;
    }

    float DamageToDeal = FMath::RoundToFloat(MinMaxDamage.GetRandomValueInRange()) * ResistanceModifier * CriticalMultiplier;
    Target->SetCurrentHealth(Target->GetCurrentHealth() - DamageToDeal);
    UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("%s deals %f damage to %s (%f health remaining)"),
		*SkillOwner->GetName(),
        DamageToDeal,
		*Target->GetName(),
        Target->GetCurrentHealth()
	));
}
