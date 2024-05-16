// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "DataAssets/AutobattlerConfiguration.h"

/* Autobattler includes. */
#include "Core/AutobattlerSettings.h"
#include "Core/AutobattlerManager.h"
#include "Utility/AutobattlerFunctionLibrary.h"

UAutobattlerConfiguration::UAutobattlerConfiguration()
{
    GridSize = 200.0f;
    GridDimensions = FIntPair(8, 8);

    PreviewCharacterRotation = FRotator(0.0f, 270.0f, 0.0f);
    EnemyCharacterRotation = FRotator(0.0f, 90.0f, 0.0f);
    CustomDepthStencil = 1;

    FResistanceContainer PierceResistances;
    PierceResistances.Resistances.Add(EResistanceType::Parry, 0.75f);
    PierceResistances.Resistances.Add(EResistanceType::Fortified, 1.05f);
    PierceResistances.Resistances.Add(EResistanceType::Absorb, 1.25f);
    PierceResistances.Resistances.Add(EResistanceType::Swift, 1.00f);
    DamageModifierMap.Add(EDamageType::Pierce, PierceResistances);

    FResistanceContainer BludgeonResistances;
    BludgeonResistances.Resistances.Add(EResistanceType::Parry, 1.15f);
    BludgeonResistances.Resistances.Add(EResistanceType::Fortified, 0.80f);
    BludgeonResistances.Resistances.Add(EResistanceType::Absorb, 0.85f);
    BludgeonResistances.Resistances.Add(EResistanceType::Swift, 1.20f);
    DamageModifierMap.Add(EDamageType::Bludgeon, BludgeonResistances);

    FResistanceContainer ImpactResistances;
    ImpactResistances.Resistances.Add(EResistanceType::Parry, 1.15f);
    ImpactResistances.Resistances.Add(EResistanceType::Fortified, 1.15f);
    ImpactResistances.Resistances.Add(EResistanceType::Absorb, 0.90f);
    ImpactResistances.Resistances.Add(EResistanceType::Swift, 0.80f);
    DamageModifierMap.Add(EDamageType::Slash, ImpactResistances);

    FResistanceContainer PureResistances;
    PureResistances.Resistances.Add(EResistanceType::Parry, 1.00f);
    PureResistances.Resistances.Add(EResistanceType::Fortified, 1.00f);
    PureResistances.Resistances.Add(EResistanceType::Absorb, 1.00f);
    PureResistances.Resistances.Add(EResistanceType::Swift, 1.00f);
    DamageModifierMap.Add(EDamageType::Impact, PureResistances);

    ProjectileMinDistanceToHit = 32.0f;
    PoisonTickRate = 2.0f;
    PoisonStrengthReductionRate = 5.0f;
}

const UAutobattlerConfiguration* UAutobattlerConfiguration::GetConfigurationAsset(const UObject* WorldContextObject)
{
    if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(WorldContextObject))
    {
        return Manager->GetAutobattlerConfigurationAsset();
    }
    else UAutobattlerFunctionLibrary::PrintErrorToLog(FString("AutobattlerConfiguration : [GetConfigurationAsset] Could not get autobattler manager!")); 

	return nullptr;
}
