// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Misc/CharacterPanelActor.h"

/* Autobattler includes. */
#include "Game/Components/CharacterPanelComponent.h"

/* Engine includes. */
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

ACharacterPanelActor::ACharacterPanelActor()
{
	CharacterPanel = CreateDefaultSubobject<UCharacterPanelComponent>(TEXT("CharacterPanelComponent"));
	bReplicates = true;
}

void ACharacterPanelActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterPanelActor, OwnerID);
}

ACharacterPanelActor* ACharacterPanelActor::GetCharacterPanelActorByID(const UObject* WorldContextObject, int32 ID)
{
	if (GEngine == nullptr) return nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<ACharacterPanelActor> ActorItr(World); ActorItr; ++ActorItr)
		{
			ACharacterPanelActor* CurrentPanelActor = *ActorItr;
			if (!IsValid(CurrentPanelActor)) continue;

			if (CurrentPanelActor->GetOwnerID() == ID) return CurrentPanelActor;
		}
	}

	return nullptr;
}

void ACharacterPanelActor::BuildCharacterPanelActor(TSubclassOf<UCharacterPanel> CharacterPanelClass, const FAutobattlerCharacterDefinition& NewDefinition, int32 NewID)
{
	OwnerID = NewID;
	CharacterPanel->BuildFromCharacterDefinition(CharacterPanelClass, NewDefinition);
}
