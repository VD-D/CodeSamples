// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Core/AutobattlerControllerComponent.h"

/* Autobattler includes. */
#include "Core/AutobattlerManager.h"
#include "DataAssets/AutobattlerConfiguration.h"
#include "Game/Grid/AutobattlerGrid.h"
#include "Game/Misc/RedeployPreview.h"
#include "Game/Player/AutobattlerPawn.h"
#include "Game/Player/PlayerAbilityAutoTarget.h"
#include "Game/Player/PlayerAbilityGetTarget.h"
#include "Game/Player/PlayerAbilityManualTarget.h"
#include "Game/Player/PlayerExecuteAbility.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Game/Units/AutobattlerPreviewCharacter.h"
#include "Interfaces/AutobattlerMouseInterface.h"
#include "UI/CharacterSelect/CharacterSelectContainerWidget.h"
#include "UI/HUD/AutobattlerHUD.h"
#include "UI/PlayerAbility/PlayerAbilityContainer.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

UAutobattlerControllerComponent::UAutobattlerControllerComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	CurrentPhase = EAutobattlerPhase::Setup;
}

void UAutobattlerControllerComponent::SetIdentity(EEntity NewIdentity)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return;

	Identity = NewIdentity;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
	{
		if (AAutobattlerPawn* AutobattlerPawn = Cast<AAutobattlerPawn>(PlayerController->GetPawn()))
		{
			AutobattlerPawn->WhoOwns = Identity;
		}
	}

	OnIdentityChanged.Broadcast(Identity);
}

void UAutobattlerControllerComponent::BindManagerDelegates()
{
	if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
	{
		Manager->OnAddedCharacterForPlayer.AddDynamic(this, &UAutobattlerControllerComponent::OnCharacterAdded);
		Manager->OnRemovedCharacterForPlayer.AddDynamic(this, &UAutobattlerControllerComponent::OnCharacterRemoved);
		Manager->OnGamePhaseAdvanced.AddDynamic(this, &UAutobattlerControllerComponent::OnGamePhaseAdvanced);
		Manager->OnFloatBegun.AddDynamic(this, &UAutobattlerControllerComponent::OnFloatStateBegun);
		Manager->OnFloatCancelled.AddDynamic(this, &UAutobattlerControllerComponent::OnFloatStateCancelled);
		Manager->OnMaxBudgetChanged.AddDynamic(this, &UAutobattlerControllerComponent::OnBudgetUpdate);
		Manager->OnCharacterDeployed.AddDynamic(this, &UAutobattlerControllerComponent::OnCharacterDeployed);
	}
	else
	{
		//Don't print warning in Test levels
		auto WorldName = GetWorld()->GetName();
		if (!WorldName.Contains(TEXT("FTEST")))
		{
			UAutobattlerFunctionLibrary::PrintWarningToLog(FString("Autobattler Manager : [Bind Manager Delegates] Could not find manager! Retrying in one second..."));

			FTimerHandle Handle;
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &UAutobattlerControllerComponent::BindManagerDelegates);
			GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, 1.0f, false);
		}
	}
}

void UAutobattlerControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	AutobattlerHUDWidget = CreateWidget<UAutobattlerHUD>(GetWorld(), AutobattlerHUDWidgetClass);
	if (IsValid(AutobattlerHUDWidget))
	{
		AutobattlerHUDWidget->AddToViewport();
		AutobattlerHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [BeginPlay] Failed to create autobattler HUD widget!"), *GetName()));

	BindManagerDelegates();
}

void UAutobattlerControllerComponent::ExecutePrimaryAction()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
	{
		FHitResult HitResult;
		bool Hit = PlayerController->GetHitResultUnderCursor(
			MouseCollisionChannel,
			false,
			HitResult
		);

		if (IsTargeting)
		{
			ConfirmTargetedAbility(HitResult);
		}
		else
		{
			for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				AActor* LiveActor = *ActorItr;
				if (!IsValid(LiveActor) || LiveActor == HitResult.GetActor()) continue;

				if (LiveActor->GetClass()->ImplementsInterface(UAutobattlerMouseInterface::StaticClass()))
				{
					IAutobattlerMouseInterface::Execute_OnDeselect(LiveActor);
				}
			}
		
			if (IsValid(HitResult.GetActor()))
			{
				if (HitResult.GetActor()->GetClass()->ImplementsInterface(UAutobattlerMouseInterface::StaticClass()))
				{
					IAutobattlerMouseInterface::Execute_OnMousePrimary(HitResult.GetActor());
				}
			}

		Server_ExecutePrimaryAction(HitResult);
		}
		
	}
}

void UAutobattlerControllerComponent::ExecuteSecondaryAction()
{
	if (IsTargeting)
	{
		CancelTargetedAbility();
	}
	else
	{
		FHitResult HitResult;
		if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
		{
			PlayerController->GetHitResultUnderCursor(
				MouseCollisionChannel,
				false,
				HitResult
			);
		}
		Server_ExecuteSecondaryAction(GetIdentity(), HitResult);
	}
}

void UAutobattlerControllerComponent::Server_ExecutePrimaryAction_Implementation(const FHitResult& MouseHitResult)
{
	AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this);
	if (!IsValid(Manager)) return;

	if (IsValid(CurrentDraggedCharacter))
	{
		if (Manager->GetCanPlayerDeployOnIndex(GetIdentity(), LastRedeploymentIndex) && !Manager->GetIsGridIndexOccupied(LastRedeploymentIndex)) 
		{
			CurrentDraggedCharacter->SetActorLocation(FVector(
				LastRedeploymentLocation.X,
				LastRedeploymentLocation.Y,
				LastRedeploymentLocation.Z + CurrentDraggedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
			);
		}
		CurrentDraggedCharacter = nullptr;
		if (IsValid(RedeployPreview)) 
		{
			RedeployPreview->Destroy();
			RedeployPreview = nullptr;
		}
		Manager->RequestChangeGridVisualisationForEntity(GetIdentity(), false);
		return;
	}
	
	if (IsValid(PreviewCharacter))
	{
		if (PreviewCharacter->GetCanBePlaced())
		{
			bool SuccessfullyDeployed = Manager->DeployCharacterForPlayerByTransform(
				GetIdentity(), 
				PreviewCharacter->GetPreviewCharacterID(),
				PreviewCharacter->GetActorTransform()
			);

			if (SuccessfullyDeployed) ClearPreviewCharacter();
			else UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("%s : [Server_ExecutePrimaryAction] Could not place preview character with ID %d for player %d"), *GetName(), PreviewCharacter->GetPreviewCharacterID(), (int32)GetIdentity()));
		}
	}
	else
	{
		if (AAutobattlerCharacter* Character = Cast<AAutobattlerCharacter>(MouseHitResult.GetActor()))
		{
			if (Character->GetOwnerIdentity() == GetIdentity() && Manager->GetGamePhase() == EAutobattlerPhase::Setup)
			{
				CurrentDraggedCharacter = Character;

				if (RedeployPreviewClass.Get() != nullptr && !IsValid(RedeployPreview))
				{
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					RedeployPreview = GetWorld()->SpawnActor<ARedeployPreview>(RedeployPreviewClass, CurrentDraggedCharacter->GetActorLocation(), FRotator::ZeroRotator, ActorSpawnParams);
					if (IsValid(RedeployPreview)) RedeployPreview->ConfigureRedployPreview(GetIdentity());
				}

				Manager->RequestChangeGridVisualisationForEntity(GetIdentity(), true);
			}
		}
	}
}

void UAutobattlerControllerComponent::Server_ExecuteSecondaryAction_Implementation(EEntity WhoCalled, const FHitResult& HitUnderMouse)
{
	bool DidCancelPreviewCharacter = false;
	AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this);
	if (IsValid(PreviewCharacter) && IsValid(Manager))
	{
		if (Manager->CancelFloatingCharacterForPlayer(GetIdentity()))
		{
			DidCancelPreviewCharacter = true;
			RequestPendingBudgetUIVisibilityChange(false);
		}
	}

	if (!DidCancelPreviewCharacter)
	{
		if (HitUnderMouse.bBlockingHit)
		{
			if (AAutobattlerCharacter* HitCharacter = Cast<AAutobattlerCharacter>(HitUnderMouse.GetActor()))
			{
				if (HitCharacter->GetOwnerIdentity() == WhoCalled)
				{
					if (HitCharacter == CurrentDraggedCharacter && IsValid(RedeployPreview))
					{
						RedeployPreview->Destroy();
						RedeployPreview = nullptr;
					}

					Manager->RemoveCharacterForPlayerByContext(HitCharacter, true);
					Manager->RequestChangeGridVisualisationForEntity(WhoCalled, false);
				}
			}
		}
	}
}

void UAutobattlerControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector NewWorldLocation = FVector::ZeroVector;
	FVector NewSnappedLocation = FVector::ZeroVector;
	FIntPair GridIndexLocation;
	bool Success = GetMouseToGridLocation(NewWorldLocation, NewSnappedLocation, GridIndexLocation);
	if (!PreviousSnappedLocation.Equals(NewSnappedLocation, 1.0f))
	{
		RPC_MouseToGridLocationChanged(GetIdentity(), Success, NewSnappedLocation, GridIndexLocation);
		PreviousSnappedLocation = NewSnappedLocation;
	}
}

void UAutobattlerControllerComponent::OnCharacterAdded(int32 ID, const FCharacterListing& NewListing, EEntity WhoOwns, bool DidGeneratePanelActor)
{
	if (WhoOwns == GetIdentity() && IsValid(AutobattlerHUDWidget) && DidGeneratePanelActor)
	{
		if (UCharacterSelectContainerWidget* CharacterSelectContainerWidget = AutobattlerHUDWidget->GetCharacterSelectContainer())
		{
			CharacterSelectContainerWidget->BuildCharacterSelectWidget(ID, WhoOwns, NewListing);
		}
	}
}

void UAutobattlerControllerComponent::OnCharacterRemoved(int32 ID, EEntity WhoOwns, bool WasReturnedToBarracks)
{
	// In this case, the character has not been completely removed, but still remains registered and needs
	// to return to barracks widget.
	if (WasReturnedToBarracks)
	{
		if (WhoOwns == GetIdentity() && IsValid(AutobattlerHUDWidget))
		{
			if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
			{
				FCharacterListing FoundListing;
				if (Manager->GetCharacterListingByID(WhoOwns, ID, FoundListing))
				{
					if (UCharacterSelectContainerWidget* CharacterSelectContainerWidget = AutobattlerHUDWidget->GetCharacterSelectContainer())
					{
						CharacterSelectContainerWidget->BuildCharacterSelectWidget(ID, WhoOwns, FoundListing);
					}
				}
				else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("AutobattlerManager : [OnCharacterRemoved] Failed to find character listing for character with id %d for player %s"), ID, *UEnum::GetValueAsString(WhoOwns)));
			}
		}
	}
	else
	// In this case, the character has been completely removed (e.g. it has died) and now needs the associated barracks entry
	// removed.
	{
		if (WhoOwns == GetIdentity() && IsValid(AutobattlerHUDWidget))
		{
			if (UCharacterSelectContainerWidget* CharacterSelectContainerWidget = AutobattlerHUDWidget->GetCharacterSelectContainer())
			{
				CharacterSelectContainerWidget->RemoveCharacterSelectWidget(ID);
			}
		}
	}

	// In either case, update the budget.
	AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this);
	if (IsValid(AutobattlerHUDWidget) && IsValid(Manager))
	{
		RequestBudgetUIUpdate(EBudgetUIType::Current, Manager->GetCurrentBudgetForEntity(WhoOwns));
	}
}

void UAutobattlerControllerComponent::OnGamePhaseAdvanced(EAutobattlerPhase NewPhase)
{
	CurrentPhase = NewPhase;
	if (IsValid(AutobattlerHUDWidget))
	{
		AutobattlerHUDWidget->SetAutobattlerGamePhaseText(NewPhase);
		AutobattlerHUDWidget->SetDeploymentEnabled(NewPhase == EAutobattlerPhase::Setup);
	}
}

void UAutobattlerControllerComponent::OnFloatStateBegun(EEntity WhoOwns, int32 CharacterID)
{
	if (WhoOwns == GetIdentity() && IsValid(AutobattlerHUDWidget))
	{
		if (UCharacterSelectContainerWidget* CharacterSelectContainerWidget = AutobattlerHUDWidget->GetCharacterSelectContainer())
		{
			CharacterSelectContainerWidget->RemoveCharacterSelectWidget(CharacterID);
		}
	}
}

void UAutobattlerControllerComponent::OnFloatStateCancelled(EEntity WhoOwns, int32 FloatedCharacterID, const FCharacterListing& FloatedCharacterListing)
{
	if (WhoOwns == GetIdentity() && IsValid(AutobattlerHUDWidget))
	{
		if (UCharacterSelectContainerWidget* CharacterSelectContainerWidget = AutobattlerHUDWidget->GetCharacterSelectContainer())
		{
			CharacterSelectContainerWidget->BuildCharacterSelectWidget(FloatedCharacterID, WhoOwns, FloatedCharacterListing);
		}
	}
}

void UAutobattlerControllerComponent::OnBudgetUpdate(EEntity WhoOwns, int32 NewBudget)
{
	if (WhoOwns == GetIdentity() && IsValid(AutobattlerHUDWidget))
	{
		RequestBudgetUIUpdate(EBudgetUIType::Max, NewBudget);
	}
}

void UAutobattlerControllerComponent::OnCharacterDeployed(EEntity WhoOwns, AAutobattlerCharacter* DeployedCharacter)
{
	if (WhoOwns == GetIdentity() && IsValid(AutobattlerHUDWidget))
	{
		RequestPendingBudgetUIVisibilityChange(false);
	}
}

void UAutobattlerControllerComponent::PlacementReasonChange(EEntity WhoOwns, EPlaceReason PlacementReasonChange)
{
	if (WhoOwns == GetIdentity() && IsValid(AutobattlerHUDWidget))
	{
		AutobattlerHUDWidget->OnPlacementReasonChange(PlacementReasonChange);
	}
}

bool UAutobattlerControllerComponent::AddPlayerAbility(const FName& AbilityName)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return false;

	if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
	{
		if (const UAutobattlerConfiguration* Configuration = Manager->GetAutobattlerConfigurationAsset())
		{
			if (const UDataTable* PlayerAbilityDatatable = Configuration->PlayerAbilityDatatable)
			{
				if (FAutobattlerPlayerAbility* PlayerAbility = PlayerAbilityDatatable->FindRow<FAutobattlerPlayerAbility>(AbilityName, FString("Autobattler Player Ability")))
				{
					PlayerAbilitiesData.Emplace(FAutobattlerPlayerAbilityData(AbilityName, PlayerAbility));
					PlayerAbilitiesCosmetic.Emplace(FAutobattlerPlayerAbilityCosmetic(AbilityName, PlayerAbility));
					UpdatePlayerAbilityUI(); // Updates on server, client updates after replication.

					return true;
				}
				else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [AddPlayerAbility] Row with name %s does not exist or row type is not FAutobattlerPlayerAbility!"), *GetName(), *AbilityName.ToString()));
			}
			else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [AddPlayerAbility] Could not get player ability datatable!"), *GetName()));
		}
		else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [AddPlayerAbility] Could not get autobattler configuration asset!"), *GetName()));
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [AddPlayerAbility] Could not get autobattler manager!"), *GetName()));

	return false;
}

bool UAutobattlerControllerComponent::RemovePlayerAbility(const FName& AbilityName)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return false;

	bool RemovedCosmetic = false;
	bool RemovedData = false;

	int32 Index = PlayerAbilitiesCosmetic.IndexOfByPredicate([AbilityName](const FAutobattlerPlayerAbilityCosmetic& Match){
		return Match.ID.IsEqual(AbilityName);
	});

	if (Index != INDEX_NONE) 
	{
		PlayerAbilitiesCosmetic.RemoveAtSwap(Index);
		UpdatePlayerAbilityUI(); // Updates on server, client updates after replication.
		RemovedCosmetic = true;
	}

	Index = PlayerAbilitiesData.IndexOfByPredicate([AbilityName](const FAutobattlerPlayerAbilityData& Match){
		return Match.ID.IsEqual(AbilityName);
	});

	if (Index != INDEX_NONE) 
	{
		PlayerAbilitiesData.RemoveAtSwap(Index);
		RemovedData = true;
	}

	return RemovedCosmetic && RemovedData;
}

void UAutobattlerControllerComponent::RequestExecutePlayerAbility(const FName& AbilityName)
{
	Server_ExecutePlayerAbility(AbilityName);
}

void UAutobattlerControllerComponent::ConfirmTargetedAbility(const FHitResult& MouseHitResult)
{
	Server_ConfirmTargetedAbility(MouseHitResult);
}

void UAutobattlerControllerComponent::CancelTargetedAbility()
{
	Server_CancelTargetedAbility();
}

void UAutobattlerControllerComponent::Server_ExecutePlayerAbility_Implementation(const FName& AbilityName)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority || CurrentPhase != EAutobattlerPhase::Fight) return;

	FAutobattlerPlayerAbilityData* PlayerAbilityData = PlayerAbilitiesData.FindByPredicate([AbilityName](const FAutobattlerPlayerAbilityData& Match){
		return Match.ID.IsEqual(AbilityName);
	});

	if (PlayerAbilityData == nullptr) return;

	if (UPlayerAbilityGetTarget* GetTargetImpl = PlayerAbilityData->GetTargetImplementation)
	{
		if (GetTargetImpl->GetClass()->IsChildOf<UPlayerAbilityAutoTarget>()) // Case where we auto target.
		{
			TArray<AAutobattlerCharacter*> TargetedCharacters;
			GetTargetImpl->PlayerGetTargets(this, TargetedCharacters);

			for (auto Target : TargetedCharacters) SpawnAndExecutePlayerAbility(PlayerAbilityData->ID, Target);

			BeginSkillCooldown(AbilityName, PlayerAbilityData->Cooldown);
		}
		else if (GetTargetImpl->GetClass()->IsChildOf<UPlayerAbilityManualTarget>()) // Case where we manual target.
		{
			TargetedAbilityID = PlayerAbilityData->ID;
			IsTargeting = true;
			RPC_SetPlayerAbilityEnabledState(false);
		}
		else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [Server_ExecutePlayerAbility_Implementation] Tried to get target implementation which is neither a child of UPlayerAbilityAutoTarget or UPlayerAbilityManualTarget!"), *GetName()));
	}
}

void UAutobattlerControllerComponent::Server_ConfirmTargetedAbility_Implementation(const FHitResult& MouseHitResult)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority || CurrentPhase != EAutobattlerPhase::Fight) return;

	FAutobattlerPlayerAbilityData* PlayerAbilityData = PlayerAbilitiesData.FindByPredicate([this](const FAutobattlerPlayerAbilityData& Match){
		return Match.ID.IsEqual(TargetedAbilityID);
	});

	if (PlayerAbilityData == nullptr)
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [Server_ConfirmTargetedAbility_Implementation] Tried to confirm skill with invalid ID! (ID = %s)"), *GetName(), *TargetedAbilityID.ToString()));
		Server_CancelTargetedAbility_Implementation();
		return;
	}

	if (UPlayerAbilityGetTarget* GetTargetImpl = PlayerAbilityData->GetTargetImplementation)
	{
		if (UPlayerAbilityManualTarget* PlayerAbilityManualTarget = Cast<UPlayerAbilityManualTarget>(GetTargetImpl))
		{
			if (PlayerAbilityManualTarget->IsTargetValid(this, MouseHitResult))
			{
				PlayerAbilityManualTarget->SetLastHitResult(MouseHitResult);

				TArray<AAutobattlerCharacter*> TargetedCharacters;
				GetTargetImpl->PlayerGetTargets(this, TargetedCharacters);

				for (auto Target : TargetedCharacters) SpawnAndExecutePlayerAbility(PlayerAbilityData->ID, Target);

				BeginSkillCooldown(TargetedAbilityID, PlayerAbilityData->Cooldown);
			}
		}
		else 
		{
			UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [Server_ConfirmTargetedAbility_Implementation] Get Skill Target Implementation is not subclass of UPlayerAbilityManualTarget! (ID = %s)"), *GetName(), *TargetedAbilityID.ToString()));
			Server_CancelTargetedAbility_Implementation();
		}
	}
	else
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [Server_ConfirmTargetedAbility_Implementation] Skill does not have get target implementation! (ID = %s)"), *GetName(), *TargetedAbilityID.ToString()));
		Server_CancelTargetedAbility_Implementation();
	}
}

void UAutobattlerControllerComponent::Server_CancelTargetedAbility_Implementation()
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;
	// Note: Ability canceling can happen outside of the fight phase, e.g. immediately when the fight ends.

	IsTargeting = false;
	TargetedAbilityID = FName("None");
	if (CurrentPhase == EAutobattlerPhase::Fight) RPC_SetPlayerAbilityEnabledState(true);
}

void UAutobattlerControllerComponent::SpawnAndExecutePlayerAbility(const FName& AbilityName, AAutobattlerCharacter* Target)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority || CurrentPhase != EAutobattlerPhase::Fight || !IsValid(Target)) return;

	FAutobattlerPlayerAbilityData* PlayerAbilityData = PlayerAbilitiesData.FindByPredicate([AbilityName](const FAutobattlerPlayerAbilityData& Match){
		return Match.ID.IsEqual(AbilityName);
	});

	if (PlayerAbilityData == nullptr) 
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [SpawnAndExecutePlayerAbility] Passed player ability was null for ability %s!"), *GetName(), *AbilityName.ToString()));
		return;
	}

	if (AbilitiesOnCooldown.Contains(PlayerAbilityData->ID))
	{
		UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("%s : [SpawnAndExecutePlayerAbility] Ability %s is on cooldown and cannot be executed!"), *GetName(), *AbilityName.ToString()));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Target->GetActorLocation());

	APlayerExecuteAbility* PlayerExecuteAbility = GetWorld()->SpawnActor<APlayerExecuteAbility>(
		PlayerAbilityData->PlayerExecuteAbilityClass,
		SpawnTransform,
		SpawnParams
	);

	if (IsValid(PlayerExecuteAbility))
	{
		PlayerExecuteAbility->ExecuteAbility(PlayerAbilityData, this, Target);
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [SpawnAndExecutePlayerAbility] Failed to execute player ability as it was null!"), *GetName()));
}

void UAutobattlerControllerComponent::BeginSkillCooldown(const FName& AbilityName, float CooldownDuration)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority || CurrentPhase != EAutobattlerPhase::Fight || !IsValid(GetWorld())) return;

	if (AbilitiesOnCooldown.Contains(AbilityName))
	{
		UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("%s : [BeginSkillCooldown] Skill with ID = %s is already on cooldown!"), *GetName(), *AbilityName.ToString()));
		return;
	}

	FAutobattlerPlayerAbilityData* PlayerAbilityData = PlayerAbilitiesData.FindByPredicate([AbilityName](const FAutobattlerPlayerAbilityData& Match){
		return Match.ID.IsEqual(AbilityName);
	});

	if (PlayerAbilityData == nullptr) 
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [BeginSkillCooldown] Passed player ability was null for ability %s!"), *GetName(), *AbilityName.ToString()));
		return;
	}

	AbilitiesOnCooldown.Emplace(AbilityName);

	FTimerHandle Handle;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, FName("OnSkillCooldownEnd"), AbilityName);

	GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, FMath::Max(CooldownDuration, 1.0f), false);
	RPC_OnAbilityCooldownBegin(AbilityName, CooldownDuration);
}

void UAutobattlerControllerComponent::OnSkillCooldownEnd(const FName& AbilityName)
{
	if (AbilitiesOnCooldown.Remove(AbilityName) != INDEX_NONE) OnAbilityCooldownEnd.Broadcast(AbilityName);
}

void UAutobattlerControllerComponent::ToggleAutobattlerUI(bool ShouldBeVisible)
{
	if (!IsValid(AutobattlerHUDWidget))
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [ToggleAutobattlerUI] Character Select Container Widget is invalid!"), *GetName()));
		return;
	}

	if (ShouldBeVisible)
	{
		AutobattlerHUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		AutobattlerHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UAutobattlerControllerComponent::RequestBudgetUIUpdate(EBudgetUIType BudgetUIType, int32 NewValue)
{
	if (!IsValid(AutobattlerHUDWidget))
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [RequestBudgetUIUpdate] Autobattler HUD is invalid!"), *GetName()));
		return;
	}

	switch (BudgetUIType)
	{
		case EBudgetUIType::Max:
			AutobattlerHUDWidget->SetMaxBudget(NewValue);
			break;

		case EBudgetUIType::Current:
			AutobattlerHUDWidget->SetCurrentBudget(NewValue);
			break;

		case EBudgetUIType::Pending:
			AutobattlerHUDWidget->SetPendingBudget(NewValue);
			break;
	}
}

void UAutobattlerControllerComponent::RequestPendingBudgetUIVisibilityChange(bool NewVisibility)
{
	if (!IsValid(AutobattlerHUDWidget))
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [RequestPendingBudgetUIVisibilityChange] Character Select Container Widget is invalid!"), *GetName()));
		return;
	}

	AutobattlerHUDWidget->SetPendingBudgetVisibility(NewVisibility);
}

void UAutobattlerControllerComponent::UpdatePlayerAbilityUI()
{
	if (UAutobattlerHUD* HUD = GetAutobattlerHUDWidget())
	{
		if (UPlayerAbilityContainer* PlayerAbilityContainer = HUD->GetPlayerAbilityContainer())
		{
			PlayerAbilityContainer->UpdatePlayerAbilities();
		}
	}
}

void UAutobattlerControllerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAutobattlerControllerComponent, Identity);
	DOREPLIFETIME(UAutobattlerControllerComponent, PreviewCharacter);
	DOREPLIFETIME(UAutobattlerControllerComponent, RedeployPreview);
	DOREPLIFETIME(UAutobattlerControllerComponent, PlayerAbilitiesCosmetic);
}

void UAutobattlerControllerComponent::OnRep_PlayerAbilitiesCosmetic()
{
	UpdatePlayerAbilityUI();
}

void UAutobattlerControllerComponent::OnRep_IsTargeting()
{
	//TODO:#ABCJ-2296
}

bool UAutobattlerControllerComponent::PossessAutobattlerPawn()
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return false;

	if (APlayerController* OwnerPlayerController = GetPlayerControllerOwner())
	{
		for (TActorIterator<AAutobattlerPawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AAutobattlerPawn* Pawn = *ActorItr;
			if (!IsValid(Pawn)) continue;

			if (Pawn->GetWhoShouldOwn() == GetIdentity()) 
			{
				Pawn->SetOwner(OwnerPlayerController);
				OwnerPlayerController->Possess(Pawn);
				return true;
			}
		}
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [PossessAutobattlerPawn] Does not have an owner which can be cast to Player Controller!"), *GetName()));

	UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("%s : [PossessAutobattlerPawn] There was no pawn which wanted to be possessed by player %d!"), *GetName(), (int32)GetIdentity()));
	return false;
}

bool UAutobattlerControllerComponent::SpawnPreviewCharacter(const FCharacterListing& CharacterListing, int32 ID)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority)
	{
		UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("%s : [SpawnPreviewCharacter] Attempted to spawn preview character on client!"), *GetName()));
		return false;
	}

	if (IsValid(PreviewCharacter))
	{
		UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("%s : [SpawnPreviewCharacter] Player %d is already previewing a character!"), *GetName(), (int32)GetIdentity()));
		return false;
	}

	if (FAutobattlerCharacterDefinition* CharacterDefinition = UAutobattlerFunctionLibrary::GetCharacterDefinitionFromConfigurationDatatable(this, CharacterListing.CharacterRowName))
	{
		if (CharacterDefinition->PreviewCharacterClass.Get() == nullptr) 
		{
			UAutobattlerFunctionLibrary::PrintMessageToLog(FString::Printf(TEXT("%s : [SpawnPreviewCharacter] Tried spawning preview character with null class!"), *GetName()));
			return false;
		}

		FVector NewWorldLocation = FVector::ZeroVector;
		FVector NewSnappedLocation = FVector::ZeroVector;
		FIntPair GridLocation = FIntPair();
		GetMouseToGridLocation(NewWorldLocation, NewSnappedLocation, GridLocation);

		const UAutobattlerConfiguration* Configuration = UAutobattlerConfiguration::GetConfigurationAsset(this);

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(NewSnappedLocation + CharacterDefinition->DeploymentOffset);
		SpawnTransform.SetRotation(Configuration != nullptr ? FQuat(Configuration->PreviewCharacterRotation) : FQuat::Identity);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PreviewCharacter = GetWorld()->SpawnActor<AAutobattlerPreviewCharacter>(
			CharacterDefinition->PreviewCharacterClass,
			SpawnTransform,
			SpawnParams
		);

		PreviewCharacter->ConfigurePreviewCharacter(CharacterDefinition->DeploymentOffset, ID, CharacterDefinition->BudgetCost);

		return true;
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [SpawnPreviewCharacter] Could not find character with row name %s!"), *GetName(), *CharacterListing.CharacterRowName.ToString()));

	return false;
}

bool UAutobattlerControllerComponent::ClearPreviewCharacter()
{
	if (IsValid(PreviewCharacter))
	{
		RPC_ClearCharacterPanels(false, PreviewCharacter->GetPreviewCharacterID());
		PreviewCharacter->Destroy();
		PreviewCharacter = nullptr;
		return true;
	}

	return false;
}

void UAutobattlerControllerComponent::RPC_MouseToGridLocationChanged_Implementation(EEntity WhoChanged, bool ValidLocation, const FVector& NewLocation, const FIntPair& NewGridIndex)
{
	OnMouseToGridLocationChanged.Broadcast(WhoChanged, ValidLocation, NewLocation, NewGridIndex);

	if (IsValid(CurrentDraggedCharacter))
	{
		LastRedeploymentLocation = NewLocation;
		LastRedeploymentIndex = NewGridIndex;
	}
}

void UAutobattlerControllerComponent::RPC_BeginFloatingCharacterForPlayer_Implementation(EEntity WhoOwns, int32 CharacterID)
{
	if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
	{
		Manager->BeginFloatingCharacterForPlayer(WhoOwns, CharacterID);
	}
}

void UAutobattlerControllerComponent::RPC_OnAbilityCooldownBegin_Implementation(const FName& AbilityID, float Cooldown)
{
	OnAbilityCooldownBegin.Broadcast(AbilityID, Cooldown);
}

void UAutobattlerControllerComponent::RPC_OnAbilityCooldownEnd_Implementation(const FName& AbilityID)
{
	OnAbilityCooldownEnd.Broadcast(AbilityID);
}

void UAutobattlerControllerComponent::RPC_ClearCharacterPanels_Implementation(bool Force, int32 CharacterID)
{
	UAutobattlerFunctionLibrary::ClearCharacterPanelsInViewport(this, Force, CharacterID);
}

void UAutobattlerControllerComponent::RPC_SetPlayerAbilityEnabledState_Implementation(bool NewEnabled)
{
	if (UAutobattlerHUD* HUD = GetAutobattlerHUDWidget())
	{
		if (UPlayerAbilityContainer* PlayerAbilityContainer = HUD->GetPlayerAbilityContainer())
		{
			PlayerAbilityContainer->SetPlayerAbilitiesEnabled(NewEnabled);
		}
	}
}

AAutobattlerPawn* UAutobattlerControllerComponent::GetAutobattlerPawn() const
{
	return GetPlayerControllerOwner() != nullptr ? Cast<AAutobattlerPawn>(GetPlayerControllerOwner()->GetPawn()) : nullptr;
}

UAutobattlerControllerComponent* UAutobattlerControllerComponent::GetControllerComponentByID(const UObject* WorldContextObject, EEntity WhoOwns)
{
	if (GEngine == nullptr) return nullptr;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (IsValid(World))
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* Controller = Iterator->Get();
			if (!IsValid(Controller)) continue;

			TArray<UAutobattlerControllerComponent*> CurrentControllerComponents;
			Controller->GetComponents<UAutobattlerControllerComponent>(CurrentControllerComponents);

			if (CurrentControllerComponents.Num() != 0)
			{
				if (CurrentControllerComponents[0]->GetIdentity() == WhoOwns) return CurrentControllerComponents[0];
			}
		}
	}

	return nullptr;
}

UAutobattlerControllerComponent* UAutobattlerControllerComponent::GetControllerComponentByContext(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return nullptr;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (IsValid(World))
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			TArray<UAutobattlerControllerComponent*> CurrentControllerComponents;
			PlayerController->GetComponents<UAutobattlerControllerComponent>(CurrentControllerComponents);

			return CurrentControllerComponents.IsValidIndex(0) ? CurrentControllerComponents[0] : nullptr;
		}
	}

	return nullptr;
}

APlayerController* UAutobattlerControllerComponent::GetLocalPlayerController(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return nullptr;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (IsValid(World))
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (IsValid(PlayerController) && PlayerController->IsLocalController())
			{
				return PlayerController;
			}
		}
	}

	return nullptr;
}

FTransform UAutobattlerControllerComponent::GetPreviewCharacterTransform() const
{
	return IsValid(PreviewCharacter) ? PreviewCharacter->GetTransform() : FTransform();
}

bool UAutobattlerControllerComponent::GetMouseToGridLocation(FVector& WorldLocation, FVector& SnappedLocation, FIntPair& GridIndexLocation)
{
	if (SetGameGrid())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursor(
				MouseCollisionChannel,
				false,
				HitResult
			);

			if (HitResult.bBlockingHit)
			{
				FIntPair GridIndex;
				if (GameGrid->LocationToGridIndex(HitResult.Location, GridIndex))
				{
					FVector GridLocation;
					FHitResult GridHitResult;
					if (GameGrid->GridIndexToLocation(GridIndex, GridLocation, GridHitResult, MouseCollisionChannel))
					{
						WorldLocation = HitResult.Location;
						SnappedLocation = GridHitResult.Location;
						GridIndexLocation = GridIndex;
						return true;
					}
				}
			}
		}
	}

	WorldLocation = FVector::ZeroVector;
	SnappedLocation = FVector::ZeroVector;
	GridIndexLocation = FIntPair(-1, -1);
	return false;
}

bool UAutobattlerControllerComponent::SetGameGrid()
{
	if (!IsValid(GameGrid))
	{
		if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
		{
			GameGrid = Manager->GetAutobattlerGrid();
		}
	}

	return IsValid(GameGrid);
}
