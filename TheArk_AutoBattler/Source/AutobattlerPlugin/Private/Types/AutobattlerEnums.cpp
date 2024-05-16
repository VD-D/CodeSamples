// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

#include "Types/AutobattlerEnums.h"

FString UAutobattlerEnums::DamageTypeToString(EDamageType DamageType)
{
    switch (DamageType)
    {
        case EDamageType::Impact:
            return FString("Impact");

        case EDamageType::Bludgeon:
            return FString("Bludgeon");

        case EDamageType::Pierce:
            return FString("Pierce");

        case EDamageType::Slash:
            return FString("Slash");
    }
    return FString("Unknown");
}

FString UAutobattlerEnums::ResistanceTypeToString(EResistanceType ResistanceType)
{
    switch (ResistanceType)
    {
        case EResistanceType::Absorb:
            return FString("Absorb");

        case EResistanceType::Fortified:
            return FString("Fortified");

        case EResistanceType::Parry:
            return FString("Parry");

        case EResistanceType::Swift:
            return FString("Swift");
    }
    return FString("Unknown");
}

