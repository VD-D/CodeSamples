// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Core/AutobattlerManager.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"
#include "Core/AutobattlerSettings.h"
#include "DataAssets/AutobattlerConfiguration.h"
#include "Game/Components/CharacterPanelComponent.h"
#include "Game/Grid/AutobattlerGrid.h"
#include "Game/Player/AutobattlerPawn.h"
#include "Game/Misc/CharacterPanelActor.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Game/Units/AutobattlerPreviewCharacter.h"
#include "Game/WinCondition/WinConditionBase.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"


AAutobattlerManager::AAutobattlerManager()
{
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	RootComponent = Billboard;

	IDDispenser = INT_MIN;
	ShouldCheckVictoryCondition = true;
	BattleEndDelay = 4.0f;

	bReplicates = true;
}

void AAutobattlerManager::Setup(int32 InNumberOfPlayers)
{
	if (bAlreadySetup) return;

	if (IsValid(GetWorld()))
	{
		SetOwner(GetWorld()->GetFirstPlayerController());
	}

	SetNumberOfPlayers(InNumberOfPlayers);
	AssignIdentities(GetWorld());
	bAlreadySetup = true;
}

void AAutobattlerManager::BeginPlay()
{
	Super::BeginPlay();

}

void AAutobattlerManager::AssignIdentities(UWorld* WorldContext)
{
	if (!IsValid(WorldContext) || !HasAuthority()) return;

	TArray<UAutobattlerControllerComponent*> ControllerComponents;

	for (FConstPlayerControllerIterator Iterator = WorldContext->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* Controller = Iterator->Get();
		if (!IsValid(Controller)) continue;

		UAutobattlerControllerComponent* ControllerComponent = Cast<UAutobattlerControllerComponent>(Controller->GetComponentByClass(UAutobattlerControllerComponent::StaticClass()));
		if (IsValid(ControllerComponent)) ControllerComponents.Emplace(ControllerComponent);
	}

	if (ControllerComponents.Num() != NumberOfPlayers)
	{
		UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("Found %d autobattler controller components but expected %d players!"), ControllerComponents.Num(), NumberOfPlayers));

		FTimerHandle Handle;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "AssignIdentities", WorldContext);
		WorldContext->GetTimerManager().SetTimer(Handle, Delegate, 1.0f, false);
		return;
	}

	TArray<EEntity> Identities = { EEntity::PlayerOne, EEntity::PlayerTwo };
	for (int32 i = 0; i < Identities.Num(); i++)
	{
		if (ControllerComponents.IsValidIndex(i) && Identities.IsValidIndex(i))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s : Setting identity %s"),
				*ControllerComponents[i]->GetName(),
				*UEnum::GetValueAsString(Identities[i]));
			ControllerComponents[i]->SetIdentity(Identities[i]);
			if (PossessAutobattlerPawnsOnAssignIdentity) ControllerComponents[i]->PossessAutobattlerPawn();
		}
	}
}

AAutobattlerManager* AAutobattlerManager::GetManager(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AAutobattlerManager> ActorItr(World); ActorItr; ++ActorItr)
		{
			AAutobattlerManager* Manager = *ActorItr;
			if (IsValid(Manager)) return Manager;
		}
	}

	return nullptr;
}

int32 AAutobattlerManager::GetMaxBudgetForEntity(EEntity WhoOwns) const
{
	if (const FIdentityConfiguration* Configuration = IdentityConfigurations.Find(WhoOwns))
	{
		return Configuration->Budget;
	}

	return 0;
}

int32 AAutobattlerManager::GetCurrentBudgetForEntity(EEntity WhoOwns) const
{
	int32 OutBudget = 0;
	TArray<AAutobattlerCharacter*> OwnedCharacters;
	GetDeployedCharactersByEntity(OwnedCharacters, WhoOwns);

	for (auto OwnedCharacter : OwnedCharacters) OutBudget += OwnedCharacter->GetBudgetCost();
	return OutBudget;
}

bool AAutobattlerManager::GetCharacterListingByID(EEntity WhoOwns, int32 ID, FCharacterListing& FoundListing) const
{
	if (!HasAuthority()) return false;

	if (const FIdentityConfiguration* IdentityConfiguration = IdentityConfigurations.Find(WhoOwns))
	{
		if (const FCharacterListing* OutListing = IdentityConfiguration->Characters.Find(ID))
		{
			FoundListing = *OutListing;
			return true;
		}
	}

	FoundListing = FCharacterListing();
	return false;
}

void AAutobattlerManager::GetDeployedCharactersByEntity(TArray<AAutobattlerCharacter*>& DeployedCharacters, EEntity WhoOwns) const
{
	DeployedCharacters.Empty();
	for (TActorIterator<AAutobattlerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AAutobattlerCharacter* Character = *ActorItr;
		if (!IsValid(Character)) continue;

		if (Character->GetOwnerIdentity() == WhoOwns)
		{
			DeployedCharacters.Emplace(Character);
		}
	}
}

void AAutobattlerManager::GetIDsOfDeployedCharacters(TArray<int32>& DeployedCharacterIDs, EEntity WhoOwns) const
{
	DeployedCharacterIDs.Empty();
	for (TActorIterator<AAutobattlerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AAutobattlerCharacter* Character = *ActorItr;
		if (!IsValid(Character)) continue;

		if (Character->GetOwnerIdentity() == WhoOwns)
		{
			DeployedCharacterIDs.Emplace(Character->GetID());
		}
	}
}

bool AAutobattlerManager::GetWhoOwnsByID(int32 ID, EEntity& WhoOwns) const
{
	for (auto& IdentityConfiguration : IdentityConfigurations)
	{
		TArray<int32> IDs;
		IdentityConfiguration.Value.Characters.GetKeys(IDs);
		if (IDs.Contains(ID))
		{
			WhoOwns = IdentityConfiguration.Key;
			return true;
		}
	}

	return false;
}

bool AAutobattlerManager::GetIsCharacterDeployed(int32 ID, AAutobattlerCharacter*& DeployedCharacter) const
{
	for (TActorIterator<AAutobattlerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AAutobattlerCharacter* Character = *ActorItr;
		if (!IsValid(Character)) continue;

		if (Character->GetID() == ID)
		{
			DeployedCharacter = Character;
			return true;
		}
	}

	DeployedCharacter = nullptr;
	return false;
}

bool AAutobattlerManager::GetIsGridIndexOccupied(const FIntPair& IndexToTest) const
{
	if (!HasAuthority()) return false;
	if (!IsValid(GetWorld()) || !IsValid(AutobattlerGrid)) return false;

	FVector GridLocation;
	FHitResult HitResult;
	ECollisionChannel CharacterCollisionChannel = GetDefault<UAutobattlerSettings>() != nullptr ? GetDefault<UAutobattlerSettings>()->CharacterCollisionChannel.GetValue() : ECollisionChannel::ECC_Pawn;
	if (!AutobattlerGrid->GridIndexToLocation(IndexToTest, GridLocation, HitResult, CharacterCollisionChannel)) return false;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(AutobattlerGrid);
	CollisionQueryParams.bTraceComplex = false;

	GetWorld()->SweepMultiByChannel(
		HitResults,
		FVector(GridLocation.X, GridLocation.Y, GridLocation.Z + AutobattlerGrid->GetGridMaxZExtent()),
		FVector(GridLocation.X, GridLocation.Y, GridLocation.Z - AutobattlerGrid->GetGridMaxZExtent()),
		FQuat(AutobattlerGrid->GetActorRotation()),
		CharacterCollisionChannel,
		FCollisionShape::MakeBox(FVector(AutobattlerGrid->GetGridXYSize() / 2.0f, AutobattlerGrid->GetGridXYSize() / 2.0f, AutobattlerGrid->GetGridMaxZExtent()))
	);

	return HitResults.ContainsByPredicate([](const FHitResult& HitResult) {
		return IsValid(Cast<AAutobattlerCharacter>(HitResult.GetActor()));
		});
}

bool AAutobattlerManager::GetIsBoundsOccupied(const FVector& BoxCenter, const FRotator& BoxRotation, const FVector& BoxExtent, TEnumAsByte<ECollisionChannel> CollisionChannel, const TArray<AActor*>& ActorsToIgnore) const
{
	if (!IsValid(GetWorld())) return false;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActors(ActorsToIgnore);

	return GetWorld()->SweepMultiByChannel(
		HitResults,
		BoxCenter,
		BoxCenter,
		FQuat(BoxRotation),
		CollisionChannel.GetValue(),
		FCollisionShape::MakeBox(BoxExtent),
		CollisionQueryParams
	);
}

bool AAutobattlerManager::GetCanPlayerDeployOnIndex(EEntity WhoOwns, const FIntPair& IndexToTest) const
{
	if (const FIdentityConfiguration* Configuration = IdentityConfigurations.Find(WhoOwns))
	{
		return Configuration->AllowedDeploymentIndicies.Contains(IndexToTest);
	}

	return false;
}

void AAutobattlerManager::AdvanceGamePhase()
{
	if (!HasAuthority()) return;

	if (GamePhase == EAutobattlerPhase::Setup) GamePhase = EAutobattlerPhase::Fight;
	else if (GamePhase == EAutobattlerPhase::Fight) GamePhase = EAutobattlerPhase::Setup;

	if (GamePhase != EAutobattlerPhase::Setup)
	{
		TArray<FIntPair> DummyArray;
		TArray<EEntity> Entities = { EEntity::PlayerOne, EEntity::PlayerTwo };
		for (auto Entity : Entities) Multicast_RequestIndexVisibilityChange(AutobattlerGrid, Entity, false, DummyArray);
	}

	Multicast_OnGamePhaseAdvance(GamePhase);
}

void AAutobattlerManager::CheckWinCondition()
{
	if (!HasAuthority())
	{
		UAutobattlerFunctionLibrary::PrintWarningToLog(FString("Autobattler Manager : [CheckWinCondition] Win condition should not be checked on client!"));
		return;
	}

	if (GetGamePhase() != EAutobattlerPhase::Fight || !ShouldCheckVictoryCondition) return;


	if (WinConditionClass.Get() == nullptr)
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString("Autobattler Manager : [CheckWinCondition] WinCondition class invalid!"));
		return;
	}

	TArray<AAutobattlerCharacter*> Characters;
	for (TActorIterator<AAutobattlerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AAutobattlerCharacter* Character = *ActorItr;
		if (!IsValid(Character)) continue;

		Characters.Emplace(Character);
	}

	UWinConditionBase* WinCondition = NewObject<UWinConditionBase>(this, WinConditionClass);
	EWhoWins Winner = WinCondition->CheckWinCondition(Characters);

	if (Winner != EWhoWins::Nobody)
	{
		if (BattleEndDelay <= 0.0f)
		{
			AdvanceGamePhase();
			ClearBattlefield(true);
		}
		else
		{
			ShouldCheckVictoryCondition = false;

			FTimerHandle Handle;
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &AAutobattlerManager::DelayedBattleEnd);
			GetWorldTimerManager().SetTimer(Handle, Delegate, BattleEndDelay, false);
		}

		Multicast_OnBattleEnd(Winner, Characters);
	}
}

void AAutobattlerManager::SetBudgetForEntity(EEntity WhoOwns, int32 NewBudget)
{
	int32 ClampedBudget = FMath::Max(NewBudget, 1);
	if (FIdentityConfiguration* Configuration = IdentityConfigurations.Find(WhoOwns))
	{
		Configuration->Budget = ClampedBudget;
	}
	else
	{
		FIdentityConfiguration NewConfiguration;
		NewConfiguration.Budget = ClampedBudget;
		IdentityConfigurations.Emplace(WhoOwns, NewConfiguration);
	}

	Multicast_OnMaxBudgetChange(WhoOwns, ClampedBudget);
}

void AAutobattlerManager::ModifyBudgetForEntity(EEntity WhoOwns, int32 DeltaBudget)
{
	int32 UpdatedBudget = 0;
	if (FIdentityConfiguration* Configuration = IdentityConfigurations.Find(WhoOwns))
	{
		UpdatedBudget = FMath::Max(Configuration->Budget + DeltaBudget, 0);
		Configuration->Budget = UpdatedBudget;
	}
	else
	{
		FIdentityConfiguration NewConfiguration;
		UpdatedBudget = FMath::Max(DeltaBudget, 0);
		NewConfiguration.Budget = UpdatedBudget;
		IdentityConfigurations.Emplace(WhoOwns, NewConfiguration);
	}

	Multicast_OnMaxBudgetChange(WhoOwns, UpdatedBudget);
}

bool AAutobattlerManager::AddDeploymentIndexForPlayer(EEntity WhoOwns, const FIntPair& NewIndex)
{
	if (!HasAuthority() || !IsValid(AutobattlerGrid)) return false;
	if (!AutobattlerGrid->IsValidGridIndex(NewIndex)) return false;

	if (FIdentityConfiguration* IdentityConfiguration = IdentityConfigurations.Find(WhoOwns))
	{
		IdentityConfiguration->AllowedDeploymentIndicies.Emplace(NewIndex);
	}
	else
	{
		FIdentityConfiguration NewConfiguration = FIdentityConfiguration();
		NewConfiguration.AllowedDeploymentIndicies.Emplace(NewIndex);
		IdentityConfigurations.Emplace(WhoOwns, NewConfiguration);
	}

	return true;
}

bool AAutobattlerManager::RemoveDeploymentIndexForPlayer(EEntity WhoOwns, const FIntPair& IndexToRemove)
{
	if (FIdentityConfiguration* IdentityConfiguration = IdentityConfigurations.Find(WhoOwns))
	{
		return IdentityConfiguration->AllowedDeploymentIndicies.Remove(IndexToRemove) > 0;
	}

	return false;
}

bool AAutobattlerManager::AddCharacterForPlayer(const FCharacterListing& NewListing, EEntity WhoOwns, int32& ID, bool GeneratePanelActor)
{
	if (!HasAuthority()) return false;

	if (FIdentityConfiguration* IdentityConfiguration = IdentityConfigurations.Find(WhoOwns))
	{
		IdentityConfiguration->Characters.Emplace(IDDispenser, NewListing);
	}
	else
	{
		FIdentityConfiguration NewConfiguration = FIdentityConfiguration();
		NewConfiguration.Characters.Emplace(IDDispenser, NewListing);
		IdentityConfigurations.Emplace(WhoOwns, NewConfiguration);
	}

	ID = IDDispenser;
	IDDispenser += 1;

	if (GeneratePanelActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (FAutobattlerCharacterDefinition* Definition = UAutobattlerFunctionLibrary::GetCharacterDefinitionFromConfigurationDatatable(this, NewListing.CharacterRowName))
		{
			if (const UAutobattlerConfiguration* Configuration = GetAutobattlerConfigurationAsset())
			{
				ACharacterPanelActor* NewPanelActor = GetWorld()->SpawnActor<ACharacterPanelActor>(ACharacterPanelActor::StaticClass(), SpawnParams);
				NewPanelActor->BuildCharacterPanelActor(Configuration->CharacterPanelWidgetClass, *Definition, ID);
			}
		}
	}

	Multicast_OnAddCharacterForPlayer(ID, NewListing, WhoOwns, GeneratePanelActor);

	return true;
}

bool AAutobattlerManager::AddCharacterForPlayerNoModifiers(const FName& NewRowName, EEntity WhoOwns, int32& ID, bool GeneratePanelActor)
{
	FCharacterListing NewListing;
	NewListing.CharacterRowName = NewRowName;
	return AddCharacterForPlayer(NewListing, WhoOwns, ID, GeneratePanelActor);
}

bool AAutobattlerManager::RemoveCharacterForPlayer(EEntity WhoOwns, int32 ID, bool ReturnToBarracks)
{
	if (!HasAuthority()) return false;

	if (ReturnToBarracks)
	{
		AAutobattlerCharacter* CharacterToReturn = nullptr;
		bool Found = GetIsCharacterDeployed(ID, CharacterToReturn);
		if (IsValid(CharacterToReturn) && Found && CharacterToReturn->GetOwnerIdentity() == WhoOwns)
		{
			CharacterToReturn->GetCharacterPanelComponent()->DestroyComponent();
			CharacterToReturn->Destroy();
			Multicast_OnRemoveCharacterForPlayer(ID, WhoOwns, ReturnToBarracks);
			return true;
		}
	}
	else
	{
		if (FIdentityConfiguration* IdentityConfiguration = IdentityConfigurations.Find(WhoOwns))
		{
			if (FCharacterListing* Listing = IdentityConfiguration->Characters.Find(ID))
			{
				AAutobattlerCharacter* CharacterToReturn = nullptr;
				bool Found = GetIsCharacterDeployed(ID, CharacterToReturn);
				if (IsValid(CharacterToReturn) && Found && CharacterToReturn->GetOwnerIdentity() == WhoOwns)
				{
					CharacterToReturn->GetCharacterPanelComponent()->DestroyComponent();
					CharacterToReturn->Destroy();
				}

				ACharacterPanelActor* CharacterPanelActorToRemove = ACharacterPanelActor::GetCharacterPanelActorByID(this, ID);
				if (IsValid(CharacterPanelActorToRemove)) CharacterPanelActorToRemove->Destroy();

				Multicast_OnRemoveCharacterForPlayer(ID, WhoOwns, ReturnToBarracks);
				return true;
			}
		}
	}

	return false;
}

bool AAutobattlerManager::RemoveCharacterForPlayerByContext(AAutobattlerCharacter* CharacterToRemove, bool ReturnToBarracks)
{
	if (!HasAuthority() || !IsValid(CharacterToRemove)) return false;

	return RemoveCharacterForPlayer(CharacterToRemove->GetOwnerIdentity(), CharacterToRemove->GetID(), ReturnToBarracks);
}

void AAutobattlerManager::BeginFloatingCharacterForPlayer(EEntity WhoOwns, int32 CharacterID)
{
	if (FIdentityConfiguration* IdentityConfiguration = IdentityConfigurations.Find(WhoOwns))
	{
		FCharacterListing OutListing;
		if (GetCharacterListingByID(WhoOwns, CharacterID, OutListing))
		{
			if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByID(this, WhoOwns))
			{
				if (ControllerComponent->SpawnPreviewCharacter(OutListing, CharacterID))
				{
					Multicast_OnFloatStateChange(WhoOwns, true, CharacterID);
					Multicast_RequestIndexVisibilityChange(AutobattlerGrid, WhoOwns, true, IdentityConfiguration->AllowedDeploymentIndicies);
				}
				else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("Autobattler Manager : [BeginFloatingCharacterForPlayer] Failed to spawn character for player %d with ID: %d"), (int32)WhoOwns, CharacterID));
			}
			else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("Autobattler Manager : [BeginFloatingCharacterForPlayer] Could not find controller component for player %d"), (int32)WhoOwns));
		}
		else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("Autobattler Manager : [BeginFloatingCharacterForPlayer] Could not find character listing for character with ID: %d"), CharacterID));
	}
	else UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("Autobattler Manager : [BeginFloatingCharacterForPlayer] No identity configuration for player %d (maybe no character was ever added?)"), (int32)WhoOwns));
}

bool AAutobattlerManager::CancelFloatingCharacterForPlayer(EEntity WhoOwns)
{
	if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByID(this, WhoOwns))
	{
		if (AAutobattlerPreviewCharacter* PreviewCharacter = ControllerComponent->GetPreviewCharacter())
		{
			int32 PreviewCharacterID = PreviewCharacter->GetPreviewCharacterID();
			FCharacterListing PreviewCharacterListing;
			if (GetCharacterListingByID(WhoOwns, PreviewCharacterID, PreviewCharacterListing))
			{
				ControllerComponent->ClearPreviewCharacter();
				Multicast_OnFloatStateCancelled(WhoOwns, PreviewCharacterListing, PreviewCharacterID);

				TArray<FIntPair> DummyArray;
				Multicast_RequestIndexVisibilityChange(AutobattlerGrid, WhoOwns, false, DummyArray);
				return true;
			}
		}
	}

	return false;
}

bool AAutobattlerManager::DeployCharacterForPlayerByGridIndex(EEntity WhoOwns, int32 CharacterID, const FIntPair& GridIndex, const FRotator& Rotation, const FVector& Scale)
{
	if (IsValid(AutobattlerGrid))
	{
		FVector Location;
		FHitResult HitResult;
		if (AutobattlerGrid->GridIndexToLocation(GridIndex, Location, HitResult)) // TODO: Read hit channel from config.
		{
			FTransform DeployTransform;
			DeployTransform.SetLocation(HitResult.Location);
			DeployTransform.SetRotation(FQuat(Rotation));
			DeployTransform.SetScale3D(Scale);

			return DeployCharacterForPlayerByTransform(WhoOwns, CharacterID, DeployTransform);
		}
		else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("Autobattler Manager : [DeployCharacterForPlayerByGridIndex] %s is not a valid grid location!"), *GridIndex.ToString()));
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("Autobattler Manager : [DeployCharacterForPlayerByGridIndex] No valid autobattler grid!")));
	return false;
}

bool AAutobattlerManager::DeployCharacterForPlayerByTransform(EEntity WhoOwns, int32 CharacterID, const FTransform& DeployTransform)
{
	if (GetGamePhase() != EAutobattlerPhase::Setup) return false;

	if (FIdentityConfiguration* Identity = IdentityConfigurations.Find(WhoOwns))
	{
		if (FCharacterListing* CurrentListing = Identity->Characters.Find(CharacterID))
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FTransform ActualDeployTransform = DeployTransform;
			UClass* AutobattlerCharacterClass = nullptr;
			if (FAutobattlerCharacterDefinition* CharacterDefinition = UAutobattlerFunctionLibrary::GetCharacterDefinitionFromConfigurationDatatable(this, CurrentListing->CharacterRowName))
			{
				if (WhoOwns == EEntity::AI) ActualDeployTransform.SetLocation(ActualDeployTransform.GetLocation() + CharacterDefinition->DeploymentOffset);
				ActualDeployTransform.SetRotation(DeployTransform.GetRotation());
				AutobattlerCharacterClass = CharacterDefinition->CharacterClass;
			}

			AAutobattlerCharacter* NewCharacter = GetWorld()->SpawnActor<AAutobattlerCharacter>(
				AutobattlerCharacterClass != nullptr ? AutobattlerCharacterClass : AAutobattlerCharacter::StaticClass(),
				ActualDeployTransform,
				ActorSpawnParams
			);

			NewCharacter->BuildCharacterFromListing(WhoOwns, CharacterID, *CurrentListing);
			NewCharacter->SetActorLocation(
				FVector(
					NewCharacter->GetActorLocation().X,
					NewCharacter->GetActorLocation().Y,
					NewCharacter->GetActorLocation().Z + NewCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
				)
			);

			if (UBoxComponent* CharacterDeploymentBounds = NewCharacter->GetDeploymentBounds())
			{
				TArray<AActor*> IgnoreActors;
				IgnoreActors.Emplace(NewCharacter);

				if (GetIsBoundsOccupied(
					CharacterDeploymentBounds->GetComponentLocation(),
					CharacterDeploymentBounds->GetComponentRotation(),
					CharacterDeploymentBounds->GetScaledBoxExtent(),
					IsValid(GetAutobattlerConfigurationAsset()) ? GetAutobattlerConfigurationAsset()->GridOccupancyCollisionChannel : ECollisionChannel::ECC_Visibility,
					IgnoreActors
				))
				{
					RemoveCharacterForPlayerByContext(NewCharacter);
					UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("When deploying character, it could not be deployed as there was a blocking area (another character or volume)!")));
					return false;
				}
			}

			if (!NewCharacter->ActionChanged.IsAlreadyBound(this, &AAutobattlerManager::OnAnyCharacterStateChange)) NewCharacter->ActionChanged.AddDynamic(this, &AAutobattlerManager::OnAnyCharacterStateChange);
			if (!NewCharacter->OnDestroyed.IsAlreadyBound(this, &AAutobattlerManager::OnCharacterDestroyed)) NewCharacter->OnDestroyed.AddDynamic(this, &AAutobattlerManager::OnCharacterDestroyed);
			Multicast_OnCharacterDeploy(WhoOwns, NewCharacter);

			TArray<FIntPair> DummyArray;
			Multicast_RequestIndexVisibilityChange(AutobattlerGrid, WhoOwns, false, DummyArray);

			return true;
		}
		else UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("Autobattler Manager : [DeployCharacterForPlayerByTransform] For player %d, there was not character with ID %d (maybe was deleted?)"), (int32)WhoOwns, CharacterID));
	}
	else UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("Autobattler Manager : [DeployCharacterForPlayerByTransform] No identity configuration for player %d (maybe no character was ever added?)"), (int32)WhoOwns));

	return false;
}

bool AAutobattlerManager::GenerateAIFormation(const FName& FormationRowName, const TArray<FName>& WhiteList, const TArray<FName>& Blacklist, bool ClearPrevious)
{
	if (ClearPrevious)
	{
		TArray<AAutobattlerCharacter*> ToBeRemoved;
		for (TActorIterator<AAutobattlerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AAutobattlerCharacter* PendingCharacter = *ActorItr;
			if (IsValid(PendingCharacter) && PendingCharacter->GetOwnerIdentity() == EEntity::AI) ToBeRemoved.Emplace(PendingCharacter);
		}

		for (int32 i = 0; i < ToBeRemoved.Num(); i++)
		{
			ToBeRemoved[i]->Destroy();
			ToBeRemoved[i] = nullptr;
		}
	}

	if (WhiteList.Num() < 1)
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString("AutobattlerManager : [GenerateAIFormation] When generating AI formation, whitelist is empty!"));
		return false;
	}

	FIdentityConfiguration* AIIdentity = IdentityConfigurations.Find(EEntity::AI);
	if (AIIdentity == nullptr)
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString("AutobattlerManager : [GenerateAIFormation] When generating AI formation, there is no identity configuration for player 0 (aka the AI)!"));
		return false;
	}

	if (AIIdentity->Characters.Num() < 1)
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString("AutobattlerManager : [GenerateAIFormation] When generating AI formation, player 0 (aka the AI) has no characters to deploy!"));
		return false;
	}

	if (const UAutobattlerConfiguration* Configuration = GetAutobattlerConfigurationAsset())
	{
		if (const UDataTable* Formations = Configuration->AIFormationsDatatable)
		{
			if (FAIFormation* AIFormation = Formations->FindRow<FAIFormation>(FormationRowName, FString("AI Formation")))
			{
				if (const UDataTable* AllCharacters = Configuration->AllCharactersDataTable)
				{
					// First, get ALL characters owned by the AI
					TMap<int32, FAutobattlerCharacterDefinition> AllDefinitions;
					for (auto& Character : AIIdentity->Characters)
					{
						if (FAutobattlerCharacterDefinition* CharacterDefinition = AllCharacters->FindRow<FAutobattlerCharacterDefinition>(Character.Value.CharacterRowName, FString("AI Formation")))
						{
							AllDefinitions.Add(Character.Key, *CharacterDefinition);
						}
						else UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("AutobattlerManager : [GenerateAIFormation] When generating AI formation, character with row name %s was not found and was skipped!"), *Character.Value.CharacterRowName.ToString()));
					}

					// Next, pick only characters which have a tag on the white list
					TMap<int32, FAutobattlerCharacterDefinition> BlackWhiteListFiltered;
					for (auto& Definition : AllDefinitions)
					{
						for (auto& Tag : Definition.Value.BlackWhiteListTags)
						{
							if (WhiteList.Contains(Tag))
							{
								BlackWhiteListFiltered.Emplace(Definition.Key, Definition.Value);
								break;
							}
						}
					}

					// Next, remove characters which have a tag on the black list (if a blacklist exists)
					if (Blacklist.Num() > 0)
					{
						TArray<int32> Keys;
						BlackWhiteListFiltered.GenerateKeyArray(Keys);

						for (auto Key : Keys)
						{
							if (FAutobattlerCharacterDefinition* Definition = BlackWhiteListFiltered.Find(Key))
							{
								for (auto& Tag : Definition->BlackWhiteListTags)
								{
									if (Blacklist.Contains(Tag))
									{
										BlackWhiteListFiltered.Remove(Key);
										break;
									}
								}
							}
						}
					}

					// Next, create a final definition from the budget.
					TMap<int32, FAutobattlerCharacterDefinition> Final;
					int32 AIBudget = GetMaxBudgetForEntity(EEntity::AI);
					TArray<int32> Keys;
					BlackWhiteListFiltered.GenerateKeyArray(Keys);

					// Shuffle key array.
					int32 LastIndex = Keys.Num() - 1;
					for (int32 i = 0; i <= LastIndex; i++)
					{
						int32 NewIndex = FMath::RandRange(i, LastIndex);
						if (i != NewIndex) Keys.Swap(i, NewIndex);
					}

					int32 PendingBudget = 0;
					for (auto Key : Keys)
					{
						if (FAutobattlerCharacterDefinition* Definition = BlackWhiteListFiltered.Find(Key))
						{
							if (PendingBudget + Definition->BudgetCost > AIBudget) continue;
							else
							{
								Final.Emplace(Key, *Definition);
								PendingBudget += Definition->BudgetCost;
							}
						}
					}

					TMap<int32, FAutobattlerCharacterDefinition> ToBeDeployedRandomly;
					TMap<FIntPair, FName> FormationMap = AIFormation->FormationMap;

					// Deploy characters first on their preferred deployment location
					while (Final.Num() != 0)
					{
						if (FormationMap.Num() == 0) return true; // This occurs if we have run out of spaces to deploy
						TArray<int32> KeyArray;
						Final.GenerateKeyArray(KeyArray);

						int32 ActiveKey = KeyArray[0];
						FAutobattlerCharacterDefinition* Definition = Final.Find(ActiveKey);

						bool DidFindDeploymentLocation = false;
						for (auto& DeploymentLocation : FormationMap)
						{
							if (DeploymentLocation.Value.IsEqual(Definition->PreferedDeploymentLocation))
							{
								DidFindDeploymentLocation = true;
								DeployCharacterForPlayerByGridIndex(EEntity::AI, ActiveKey, DeploymentLocation.Key, FRotator::ZeroRotator, FVector::OneVector);
								FormationMap.Remove(DeploymentLocation.Key);
								break;
							}
						}

						if (!DidFindDeploymentLocation) ToBeDeployedRandomly.Emplace(ActiveKey, *Definition);
						Final.Remove(ActiveKey);
					}

					// Otherwise, deploy characters randomly.
					if (ToBeDeployedRandomly.Num() > 0)
					{
						while (ToBeDeployedRandomly.Num() != 0)
						{
							if (FormationMap.Num() == 0) return true; // This occurs if we have run out of spaces to deploy
							TArray<int32> KeyArray;
							ToBeDeployedRandomly.GenerateKeyArray(KeyArray);

							int32 ActiveKey = KeyArray[0];
							FAutobattlerCharacterDefinition* Definition = Final.Find(ActiveKey);

							TArray<FIntPair> DeploymentLocations;
							FormationMap.GenerateKeyArray(DeploymentLocations);

							FIntPair RandomLocation = DeploymentLocations[FMath::RandRange(0, DeploymentLocations.Num() - 1)];
							DeployCharacterForPlayerByGridIndex(EEntity::AI, ActiveKey, RandomLocation, FRotator::ZeroRotator, FVector::OneVector);

							FormationMap.Remove(RandomLocation);
							ToBeDeployedRandomly.Remove(ActiveKey);
						}
					}

					return true;
				}
				else UAutobattlerFunctionLibrary::PrintErrorToLog(FString("AutobattlerManager : [GenerateAIFormation] When generating AI formation, All Characters DataTable is invalid!"));
			}
			else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("AutobattlerManager : [GenerateAIFormation] When generating AI formation, could not find formation with row name %s!"), *FormationRowName.ToString()));
		}
		else UAutobattlerFunctionLibrary::PrintErrorToLog(FString("AutobattlerManager : [GenerateAIFormation] When generating AI formation, AI Formation Datatable is invalid!"));
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString("AutobattlerManager : [GenerateAIFormation] When generating AI formation, could not get configuration asset!"));

	return false;
}

bool AAutobattlerManager::GenerateBattleFromDebugConfig(const FName& DebugBattleRowName)
{
	if (const UAutobattlerConfiguration* Configuration = GetAutobattlerConfigurationAsset())
	{
		if (const UDataTable* DebugBattleDT = Configuration->DebugBattleConfigurations)
		{
			if (FDebugBattleConfiguration* DebugBattleConfiguration = DebugBattleDT->FindRow<FDebugBattleConfiguration>(DebugBattleRowName, FString("Debug Battle Configuration")))
			{
				if (DebugBattleConfiguration->ClearAutobattlerState) ClearAutobattlerState();
				else ClearBattlefield(true);

				for (auto& EntityConfiguration : DebugBattleConfiguration->EntityConfigurations)
				{
					for (auto& SoldierConfiguration : EntityConfiguration.Value.SoldierConfigurations)
					{
						int32 NewID = 0;
						AddCharacterForPlayer(SoldierConfiguration.SoldierListing, EntityConfiguration.Key, NewID, !SoldierConfiguration.StartDeployed);
						if (SoldierConfiguration.StartDeployed)
						{
							AddDeploymentIndexForPlayer(EntityConfiguration.Key, SoldierConfiguration.DeploymentIndex);
							DeployCharacterForPlayerByGridIndex(EntityConfiguration.Key, NewID, SoldierConfiguration.DeploymentIndex, FRotator::ZeroRotator, FVector::OneVector);
						}
					}
				}

				SetBudgetForEntity(EEntity::AI, DebugBattleConfiguration->AIBudget);

				GenerateAIFormation(
					DebugBattleConfiguration->AIFormationRowName,
					DebugBattleConfiguration->AIFormationWhitelist,
					DebugBattleConfiguration->AIFormationBlacklist
				);

				return true;
			}
		}
	}
	return false;
}

bool AAutobattlerManager::GenerateAIFormationFromDatatable(const UDataTable* FormationDatatable, const FName& RowName)
{
	if (FormationDatatable == nullptr)
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString("AutobattlerManager : [GenerateAIFormationFromDatatable] When generating AI formation, passed FormationDatatable is invalid!"));
		return false;
	}

	if (FAutobattlerAIFormation* NewFormation = FormationDatatable->FindRow<FAutobattlerAIFormation>(RowName, FString("GenerateAIFormationFromDatatable")))
	{
		if (NewFormation->ClearPreviousAIState) ClearStateForEntity(EEntity::AI);
		SetBudgetForEntity(EEntity::AI, NewFormation->AIBudget);

		TArray<FName> FormationWhitelist;
		TArray<FName> FormationBlacklist;
		GenerateFormationTags(FormationWhitelist, NewFormation->WhitelistTags);
		GenerateFormationTags(FormationBlacklist, NewFormation->BlacklistTags);

		int32 DummyID = 0;
		for (auto& NewCharacterListing : NewFormation->NewAICharacters)
		{
			if (NewCharacterListing.PerCharacterChance)
			{
				for (int32 i = 1; i <= FMath::Max(NewCharacterListing.Quantity, 1); i++)
				{
					if (NewCharacterListing.Chance <= FMath::RandRange(0.001f, 1.0f)) AddCharacterForPlayerNoModifiers(NewCharacterListing.CharacterRowName, EEntity::AI, DummyID);
				}
			}
			else
			{
				if (NewCharacterListing.Chance <= FMath::RandRange(0.001f, 1.0f))
				{
					for (int32 i = 1; i <= FMath::Max(NewCharacterListing.Quantity, 1); i++)
					{
						AddCharacterForPlayerNoModifiers(NewCharacterListing.CharacterRowName, EEntity::AI, DummyID);
					}
				}
			}
		}

		return GenerateAIFormation(NewFormation->FormationRowName, FormationWhitelist, FormationBlacklist, false); // Note: state clearing needs to be done beforehand, since we're randomly generating barracks.
	}
	else
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("AutobattlerManager : [GenerateAIFormationFromDatatable] No AI formation with rowname %s exists!"), *RowName.ToString()));
		return false;
	}
	
	return false;
}

void AAutobattlerManager::ClearBattlefield(bool ReturnToBarracks)
{
	TArray<AAutobattlerCharacter*> DeployedCharacters;
	for (TActorIterator<AAutobattlerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AAutobattlerCharacter* Character = *ActorItr;
		if (!IsValid(Character)) continue;

		DeployedCharacters.Emplace(Character);
	}

	for (int32 i = 0; i < DeployedCharacters.Num(); i++)
	{
		if (DeployedCharacters[i]->GetIsDead()) RemoveCharacterForPlayerByContext(DeployedCharacters[i]);
		else RemoveCharacterForPlayerByContext(DeployedCharacters[i], ReturnToBarracks);
		DeployedCharacters[i] = nullptr;
	}
}

void AAutobattlerManager::ClearAutobattlerState()
{
	ClearBattlefield(false);
	TMap<int32, EEntity> CharactersToRemove;
	for (auto& IdentityConfiguration : IdentityConfigurations)
	{
		for (auto& Character : IdentityConfiguration.Value.Characters)
		{
			CharactersToRemove.Emplace(Character.Key, IdentityConfiguration.Key);
		}
	}

	for (auto& CharacterToRemove : CharactersToRemove)
	{
		RemoveCharacterForPlayer(CharacterToRemove.Value, CharacterToRemove.Key);
	}

	IdentityConfigurations.Empty();
}

void AAutobattlerManager::ClearStateForEntity(EEntity WhoOwns)
{
	if (!HasAuthority()) return;

	if (FIdentityConfiguration* IdentityConfiguration = IdentityConfigurations.Find(WhoOwns))
	{
		SetBudgetForEntity(WhoOwns, 0);

		TArray<int32> IDArray;
		IdentityConfiguration->Characters.GenerateKeyArray(IDArray);

		for (int32 i = 0; i < IDArray.Num(); i++)
		{
			RemoveCharacterForPlayer(WhoOwns, IDArray[i]);
		}

		IdentityConfiguration->Characters.Empty();

		TArray<FIntPair> AllowedDeploymentIndiciesTemp = IdentityConfiguration->AllowedDeploymentIndicies;

		for (int32 i = 0; i < AllowedDeploymentIndiciesTemp.Num(); i++)
		{
			RemoveDeploymentIndexForPlayer(WhoOwns, AllowedDeploymentIndiciesTemp[i]);
		}

		IdentityConfiguration->AllowedDeploymentIndicies.Empty();
	}
}

void AAutobattlerManager::RequestChangeGridVisualisationForEntity(EEntity WhoOwns, bool bGridVisible)
{
	if (FIdentityConfiguration* IdentityConfiguration = IdentityConfigurations.Find(WhoOwns))
	{
		Multicast_RequestIndexVisibilityChange(AutobattlerGrid, WhoOwns, bGridVisible, IdentityConfiguration->AllowedDeploymentIndicies);
	}
}

void AAutobattlerManager::SetNumberOfPlayers(int32 InNumberOfPlayers)
{
	NumberOfPlayers = InNumberOfPlayers;
}

void AAutobattlerManager::OnAnyCharacterStateChange(EActionType NewAction, AAutobattlerCharacter* UpdatedCharacter)
{
	if (!HasAuthority()) return;
	if (GetGamePhase() == EAutobattlerPhase::Fight)
	{
		CheckWinCondition();
	}

	if (NewAction == EActionType::Dead && IsValid(UpdatedCharacter))
	{
		Multicast_OnCharacterDeath(UpdatedCharacter->GetOwnerIdentity(), UpdatedCharacter->GetID(), UpdatedCharacter);
	}
}

void AAutobattlerManager::DelayedBattleEnd()
{
	AdvanceGamePhase();
	ClearBattlefield(true);
	ShouldCheckVictoryCondition = true;
}

void AAutobattlerManager::OnCharacterDestroyed(AActor* DestoryedActor)
{
	Multicast_ClearInvalidCharacterPanels();
}

void AAutobattlerManager::GenerateFormationTags(TArray<FName>& ChosenTags, const TArray<FTagRandomiserParameters>& RandomiserParams)
{
	ChosenTags.Empty(RandomiserParams.Num());
	for (auto& RandomiserParam : RandomiserParams)
	{
		if (RandomiserParam.Chance <= FMath::RandRange(0.001f, 1.0f)) ChosenTags.Emplace(RandomiserParam.Tag);
	}
}

void AAutobattlerManager::Multicast_OnAddCharacterForPlayer_Implementation(int32 ID, const FCharacterListing& NewCharacterListing, EEntity WhoOwns, bool DidGeneratePanelActor)
{
	OnAddedCharacterForPlayer.Broadcast(ID, NewCharacterListing, WhoOwns, DidGeneratePanelActor);
}

void AAutobattlerManager::Multicast_OnRemoveCharacterForPlayer_Implementation(int32 ID, EEntity WhoOwns, bool WasReturnedToBarracks)
{
	OnRemovedCharacterForPlayer.Broadcast(ID, WhoOwns, WasReturnedToBarracks);
}

void AAutobattlerManager::Multicast_OnGamePhaseAdvance_Implementation(EAutobattlerPhase NewPhase)
{
	OnGamePhaseAdvanced.Broadcast(NewPhase);
}

void AAutobattlerManager::Multicast_OnBattleEnd_Implementation(EWhoWins WhoWins, const TArray<AAutobattlerCharacter*>& Survivors)
{
	OnBattleEnded.Broadcast(WhoWins, Survivors);
}

void AAutobattlerManager::Multicast_OnCharacterDeploy_Implementation(EEntity WhoOwns, AAutobattlerCharacter* DeployedCharacter)
{
	OnCharacterDeployed.Broadcast(WhoOwns, DeployedCharacter);
}

void AAutobattlerManager::Multicast_OnFloatStateChange_Implementation(EEntity WhoOwns, bool Begun, int32 CharacterID)
{
	if (Begun) OnFloatBegun.Broadcast(WhoOwns, CharacterID);
	else OnFloatEnd.Broadcast(WhoOwns, CharacterID);
}

void AAutobattlerManager::Multicast_OnFloatStateCancelled_Implementation(EEntity WhoOwns, const FCharacterListing& FloatedCharacterListing, int32 CharacterID)
{
	OnFloatCancelled.Broadcast(WhoOwns, CharacterID, FloatedCharacterListing);
}

void AAutobattlerManager::Multicast_OnMaxBudgetChange_Implementation(EEntity WhoOwns, int32 NewBudget)
{
	OnMaxBudgetChanged.Broadcast(WhoOwns, NewBudget);
}

void AAutobattlerManager::Multicast_RequestIndexVisibilityChange_Implementation(AAutobattlerGrid* Grid, EEntity WhoOwns, bool Show, const TArray<FIntPair>& IndiciesToShow)
{
	if (IsValid(Grid))
	{
		if (APlayerController* Controller = UAutobattlerControllerComponent::GetLocalPlayerController(Grid))
		{
			if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerFunctionLibrary::GetFirstComponent<UAutobattlerControllerComponent>(Controller))
			{
				if (ControllerComponent->GetIdentity() == WhoOwns)
				{
					if (!Show) Grid->ToggleGridVisibility(false);
					else
					{
						Grid->ToggleGridVisibility(true);
						Grid->ShowOnlyIndicies(IndiciesToShow);
					}
				}
			}
		}
	}
}

void AAutobattlerManager::Multicast_OnCharacterDeath_Implementation(EEntity WhoOwns, int32 DeadCharacterID, AAutobattlerCharacter* DeadCharacter)
{
	OnCharacterDeath.Broadcast(WhoOwns, DeadCharacterID, DeadCharacter);
}

void AAutobattlerManager::Multicast_ClearInvalidCharacterPanels_Implementation()
{
	UAutobattlerFunctionLibrary::ClearInvalidCharacterPanels(this);
}

void AAutobattlerManager::PrintEntityConfigurationToLog(EEntity WhoOwns)
{
	FString WhoOwnsString = UEnum::GetValueAsString(WhoOwns);
	if (FIdentityConfiguration* IdentityConfiguration = IdentityConfigurations.Find(WhoOwns))
	{
		UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("Printing entity configuration for %s : "), *WhoOwnsString));
		UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("Entity %s has budget : %d"), *WhoOwnsString, IdentityConfiguration->Budget));
		UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("Entity %s can deploy on the following incidies : "), *WhoOwnsString));
		for (auto AllowedDeploymentIndex : IdentityConfiguration->AllowedDeploymentIndicies) UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("%s"), *AllowedDeploymentIndex.ToString()));
		UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("Entity %s has the following soldiers registered : "), *WhoOwnsString));
		for (auto Character : IdentityConfiguration->Characters) UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("%s with ID : %d "), *Character.Value.CharacterRowName.ToString(), Character.Key));
	}
	else
	{
		UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("When printing entity configuration to log, no configuration found for player %s!"), *WhoOwnsString));
	}
}
