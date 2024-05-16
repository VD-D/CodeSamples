// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Components/PoisonComponent.h"

/* Autobattler includes. */
#include "DataAssets/AutobattlerConfiguration.h"
#include "Game/Units/AutobattlerCharacter.h"

void UPoisonComponent::ApplyPoison(float PoisonStrength)
{
	AAutobattlerCharacter* OwnerCharacter = Cast<AAutobattlerCharacter>(GetOwner());
	const UAutobattlerConfiguration* Configuration = UAutobattlerConfiguration::GetConfigurationAsset(this);
	if (ensureMsgf(IsValid(OwnerCharacter) && IsValid(Configuration), TEXT("%s : [ApplyPoison] Either configuration is invalid or owner is not autobattler character!"), *GetName()))
	{
		float ClampedPoisonStrength = FMath::Max(0.0f, PoisonStrength);
		if (PoisonTimerHandle.IsValid())
		{
			CurrentPoisonStrength += ClampedPoisonStrength;
		}
		else
		{
			CurrentPoisonStrength = ClampedPoisonStrength;
			FTimerDelegate PoisonTimerDelegate;
			PoisonTimerDelegate.BindUFunction(this, FName("TickPoison"));
			
			GetWorld()->GetTimerManager().SetTimer(PoisonTimerHandle, PoisonTimerDelegate, Configuration->PoisonTickRate, true, 0.0f);
		}
	}
}

void UPoisonComponent::TickPoison()
{
	AAutobattlerCharacter* OwnerCharacter = Cast<AAutobattlerCharacter>(GetOwner());
	const UAutobattlerConfiguration* Configuration = UAutobattlerConfiguration::GetConfigurationAsset(this);
	if (IsValid(Configuration) && IsValid(OwnerCharacter))
	{
		OwnerCharacter->SetCurrentHealth(OwnerCharacter->GetCurrentHealth() - CurrentPoisonStrength);
		CurrentPoisonStrength -= Configuration->PoisonStrengthReductionRate;
		if (CurrentPoisonStrength <= 0.0f) DestroyComponent();
	}
}
