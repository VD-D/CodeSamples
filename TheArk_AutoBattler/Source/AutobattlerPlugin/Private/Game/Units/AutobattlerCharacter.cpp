// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Units/AutobattlerCharacter.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Animation/AutobattlerAnimInstance.h"
#include "Core/AutobattlerControllerComponent.h"
#include "Core/AutobattlerManager.h"
#include "Core/AutobattlerSettings.h"
#include "DataAssets/AutobattlerConfiguration.h"
#include "Game/Components/AutobattlerChargeComponent.h"
#include "Game/Components/AutobattlerInGameUILogicProvider.h"
#include "Game/Components/CharacterPanelComponent.h"
#include "Game/Components/CustomDepthComponent.h"
#include "UI/ChargeBar/ChargeBar.h"
#include "UI/Deployment/CharacterPanel.h"
#include "UI/HealthBar/HealthBar.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "NavModifierComponent.h"
#include "Net/UnrealNetwork.h"

AAutobattlerCharacter::AAutobattlerCharacter()
{
 	AIControllerClass = AAutobattlerAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	CurrentAction = EActionType::Idle;
	bUseControllerRotationYaw = false;
	bReplicates = true;

	DeploymentBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("DeploymentBounds"));
	DeploymentBounds->SetupAttachment(RootComponent);
	DeploymentBounds->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	HealthBarDisplay = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarDisplay"));
	HealthBarDisplay->SetupAttachment(RootComponent);
	HealthBarDisplay->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarDisplay->SetDrawAtDesiredSize(true);

	ChargeBarDisplay = CreateDefaultSubobject<UWidgetComponent>(TEXT("ChargeBarDisplay"));
	ChargeBarDisplay->SetupAttachment(RootComponent);
	ChargeBarDisplay->SetWidgetSpace(EWidgetSpace::Screen);
	ChargeBarDisplay->SetDrawAtDesiredSize(true);
	ChargeBarDisplay->SetHiddenInGame(true);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->SetRVOAvoidanceWeight(100.0f);

	GetMesh()->SetReceivesDecals(false);

	CharacterPanelComponent = CreateDefaultSubobject<UCharacterPanelComponent>(TEXT("CharacterPanelComponent"));
	CustomDepthComponent = CreateDefaultSubobject<UCustomDepthComponent>(TEXT("CustomDepthComponent"));
}

void AAutobattlerCharacter::BuildCharacterFromListing(EEntity WhoOwns, int32 CharacterID, const FCharacterListing& CharacterListing)
{
	if (!HasAuthority()) return;

	if (FAutobattlerCharacterDefinition* CharacterDefinition = UAutobattlerFunctionLibrary::GetCharacterDefinitionFromConfigurationDatatable(this, CharacterListing.CharacterRowName))
	{
		ID = CharacterID;
		BudgetCost = CharacterDefinition->BudgetCost;
		MaxHealth = CharacterDefinition->BaseHealth;
		CurrentHealth = CharacterDefinition->BaseHealth;
		CharacterResistanceType = CharacterDefinition->BaseResistanceType;
		CharacterDamageType = CharacterDefinition->BaseDamageType;
		CriticalChance = CharacterDefinition->CriticalChance;
		CriticalMultiplier = CharacterDefinition->CriticalMultiplier;
		PathAroundQuery = CharacterDefinition->PathAroundQuery;
		SurroundingPointsQuery = CharacterDefinition->SurroundingPointsQuery;
		OwnerID = WhoOwns;
		OnOwnerIDChanged(OwnerID);

		GetCharacterMovement()->MaxWalkSpeed = CharacterDefinition->MovementSpeed;
		SetActionType(EActionType::Idle);

		ECollisionChannel CharacterCollisionChannel = GetDefault<UAutobattlerSettings>() != nullptr ? GetDefault<UAutobattlerSettings>()->CharacterCollisionChannel.GetValue() : ECollisionChannel::ECC_Pawn;
		GetCapsuleComponent()->SetCollisionResponseToChannel(CharacterCollisionChannel, ECollisionResponse::ECR_Block);
		GetCapsuleComponent()->SetCollisionObjectType(CharacterCollisionChannel);

		const UAutobattlerConfiguration* Configuration = UAutobattlerConfiguration::GetConfigurationAsset(this);
		if (Configuration != nullptr)
		{
			CharacterPanelComponent->BuildFromCharacterDefinition(Configuration->CharacterPanelWidgetClass, *CharacterDefinition);
		}
		
		// NOTE R.U. : Optimalisation which removes this networking-heavy function.
		// Might completely remove in the future.
		/*
		FMaterialContainer* MaterialContainer = CharacterDefinition->MaterialOverrides.Find((EEntity)WhoOwns); // TODO: This should be changed to a uint8.
		UpdateVisuals(
			GetDefault<UAutobattlerSettings>() != nullptr ? GetDefault<UAutobattlerSettings>()->MouseCollisionChannel.GetValue() : ECC_Visibility,
			CharacterDefinition->CharacterMesh, 
			CharacterDefinition->CharaterMeshRelativeTransform, 
			CharacterDefinition->AnimInstanceClass, 
			MaterialContainer != nullptr ? *MaterialContainer : FMaterialContainer(),
			CharacterDefinition->HealthBarOffset,
			CharacterDefinition->HealthBarWidgetClass,
			CharacterDefinition->ChargeBarOffset,
			CharacterDefinition->ChargeBarWidgetClass,
			Configuration != nullptr ? Configuration->CustomDepthStencil : 0
		);
		*/

		UpdateCurrentBudget();

		if (AAutobattlerAIController* AIController = Cast<AAutobattlerAIController>(GetController()))
		{
			AIController->SetCharacterListingName(CharacterListing.CharacterRowName);
		}
	}
	else
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(
			FString::Printf(TEXT("%s : [BuildCharacterFromListing] Character Definition with row name %s does not exist!"),
			*GetName(),
			*CharacterListing.CharacterRowName.ToString()
			)
		);
		Destroy();
	}
}

void AAutobattlerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
		{
			Manager->OnGamePhaseAdvanced.AddDynamic(this, &AAutobattlerCharacter::OnGamePhaseChanged);
		}
		else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [BeginPlay] Could not get Game Manager therefore did not assign OnGamePhaseChanged!"), *GetName()));
	}
}

bool AAutobattlerCharacter::GetUsesCharges() const
{
	return UAutobattlerFunctionLibrary::HasComponent<UAutobattlerChargeComponent>(this);
}

void AAutobattlerCharacter::SetCurrentHealth(float NewHealth)
{
	float ClampedHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	bool LostHealth = ClampedHealth < CurrentHealth;
	CurrentHealth = ClampedHealth;
	UpdateHealthBar();

	if (LostHealth) OnHealthLost.Broadcast();
	else OnHealthGained.Broadcast();
	
	if (CurrentHealth == 0.0f) 
	{
		if (CanRessurect && !HasRessurected)
		{
			HasRessurected = true;
			CurrentAction = EActionType::Dead;
			SetAnimationActionType(EActionType::Ressurect);
			if (HasAuthority()) ActionChanged.Broadcast(CurrentAction, this);
		}
		else Kill();
	}
}

void AAutobattlerCharacter::SetActionType(EActionType NewAction)
{
	if (CurrentAction == NewAction || NewAction == EActionType::Dead) return;

	CurrentAction = NewAction;
	if (HasAuthority()) ActionChanged.Broadcast(CurrentAction, this);
}

void AAutobattlerCharacter::Kill()
{
	CurrentAction = EActionType::Dead;
	ToggleHealthBarVisibility(false);
	SetAnimationActionType(EActionType::Dead);
	SetActorEnableCollision(false);
	if (HasAuthority()) 
	{
		UE_LOG(LogTemp, Log, TEXT("Killed character with ID : %d"), GetID());
		ActionChanged.Broadcast(CurrentAction, this);
	}
}

void AAutobattlerCharacter::ModifyStatistic(EAutobattlerStatType StatToModify, float ModifierScale, bool IsPermanent, float ExpiryDuration)
{
	if (!HasAuthority()) return;
	ModifyStatisticInternal(StatToModify, ModifierScale);

	if (IsPermanent) return;

	FTimerHandle Handle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, StatToModify, ModifierScale, ExpiryDuration](){
		this->ModifyStatisticInternal(StatToModify, ModifierScale * -1.0f);
	});

	GetWorldTimerManager().SetTimer(Handle, TimerDelegate, ExpiryDuration, false);
}

void AAutobattlerCharacter::ModifyResistanceType(EResistanceType NewResistanceType, bool IsPermanent, float ExpiryDuration)
{
	if (!HasAuthority()) return;

	if (IsPermanent)
	{
		CharacterResistanceType = NewResistanceType;
	}
	else
	{
		FTimerHandle Handle;
		FTimerDelegate TimerDelegate;
		EResistanceType OldResistanceType = CharacterResistanceType;
		TimerDelegate.BindLambda([this, OldResistanceType](){
			this->CharacterResistanceType = OldResistanceType;
			this->UpdateResistanceUI(true);
		});

		GetWorldTimerManager().SetTimer(Handle, TimerDelegate, ExpiryDuration, false);
		CharacterResistanceType = NewResistanceType;
		UpdateResistanceUI(false);
	}
}

void AAutobattlerCharacter::ModifyDamageType(EDamageType NewDamageType, bool IsPermanent, float ExpiryDuration)
{
	if (!HasAuthority()) return;

	if (IsPermanent)
	{
		CharacterDamageType = NewDamageType;
	}
	else
	{
		FTimerHandle Handle;
		FTimerDelegate TimerDelegate;
		EDamageType OldDamageType = CharacterDamageType;
		TimerDelegate.BindLambda([this, OldDamageType](){
			this->CharacterDamageType = OldDamageType;
			this->UpdateDamageTypeUI(true);
		});

		GetWorldTimerManager().SetTimer(Handle, TimerDelegate, ExpiryDuration, false);
		CharacterDamageType = NewDamageType;
		UpdateDamageTypeUI(false);
	}
}

void AAutobattlerCharacter::ConfigureGetChargeComponent(TSubclassOf<UAutobattlerChargeComponent> ChargeComponentClass, int32 ChargesToTriggerAbility)
{
	if (!HasAuthority()) return;
	if (ensureAlwaysMsgf(ChargeComponentClass.Get() != nullptr, TEXT("%s : [ConfigureGetChargeComponent] Does not have valid charge component class!"), *GetName()))
	{
		MaxCharges = FMath::Max(ChargesToTriggerAbility, 1);

		UAutobattlerChargeComponent* NewChargeComponent = NewObject<UAutobattlerChargeComponent>(this, ChargeComponentClass);
		NewChargeComponent->RegisterComponent();
		NewChargeComponent->Initialise(this, Cast<AAutobattlerAIController>(GetController()));
	}
}

void AAutobattlerCharacter::GainCharge()
{
	Charges += 1;
	if (Charges == MaxCharges)
	{
		Charges = 0;
		AAutobattlerAIController* AIController = Cast<AAutobattlerAIController>(GetController());
		if (ensureAlwaysMsgf( IsValid(AIController), TEXT("%s : [GainCharge] Controller is not AutobattlerAIController!")))
		{
			AIController->OnAbilityCooldownEnd();
		}
	}
	UpdateChargeBar();
}

void AAutobattlerCharacter::EndRessurect()
{
	if (!HasRessurected) return;

	SetCurrentHealth(MaxHealth);
	SetActionType(EActionType::Idle);
	SetAnimationActionType(EActionType::Idle);
}

void AAutobattlerCharacter::ToggleUIElementsLocal(bool ShouldBeVisible)
{
	if (IsValid(HealthBarDisplay)) HealthBarDisplay->SetHiddenInGame(!ShouldBeVisible);
	if (IsValid(ChargeBarDisplay)) ChargeBarDisplay->SetHiddenInGame(!ShouldBeVisible);
}

void AAutobattlerCharacter::ModifyStatisticInternal(EAutobattlerStatType StatToModify, float ModifierScale)
{
	if (CurrentGamePhase != EAutobattlerPhase::Fight) return;
	switch (StatToModify)
	{
		case EAutobattlerStatType::AttackSpeed:
			AttackSpeedModifier += ModifierScale;
			break;

		case EAutobattlerStatType::SkillSpeed:
			SkillSpeedModifier += ModifierScale;
			break;

		case EAutobattlerStatType::CriticalChance:
			CriticalChance += ModifierScale;
			break;

		case EAutobattlerStatType::CriticalMultiplier:
			CriticalMultiplier += ModifierScale;
			break;

		case EAutobattlerStatType::MovementSpeed:
			GetCharacterMovement()->MaxWalkSpeed += ModifierScale;
			break;

		case EAutobattlerStatType::MaxHealth:
			MaxHealth = MaxHealth + ModifierScale;
			SetCurrentHealth(GetCurrentHealth() + ModifierScale);

		case EAutobattlerStatType::AttackRange:
			break;

		case EAutobattlerStatType::AbilityRange:

			break;

		default:
			break;
	}

	UpdateCharacterPanelStat(StatToModify, ModifierScale);
}

void AAutobattlerCharacter::UpdateCharacterPanelStat_Implementation(EAutobattlerStatType StatToModify, float ModifierScale)
{
	GetCharacterPanelComponent()->UpdateStatNumeric(StatToModify, ModifierScale, false);
}

void AAutobattlerCharacter::UpdateResistanceUI(bool IsPermanent)
{
	const UAutobattlerConfiguration* Configuration = UAutobattlerConfiguration::GetConfigurationAsset(this);
	if (ensureAlwaysMsgf(IsValid(Configuration), TEXT("%s : [UpdateResistanceUI] Could not get configuration asset!")))
	{
		const FText* ResistanceTypeText = Configuration->ResistanceTextMap.Find(CharacterResistanceType);
		UpdateDiscreteStat(EAutobattlerDiscreteType::ResistanceType, ResistanceTypeText != nullptr ? *ResistanceTypeText : FText::GetEmpty(), IsPermanent);
	}
}

void AAutobattlerCharacter::UpdateDamageTypeUI(bool IsPermanent)
{
	const UAutobattlerConfiguration* Configuration = UAutobattlerConfiguration::GetConfigurationAsset(this);
	if (ensureAlwaysMsgf(IsValid(Configuration), TEXT("%s : [UpdateDamageTypeUI] Could not get configuration asset!")))
	{
		const FText* DamageTypeText = Configuration->DamageTextMap.Find(CharacterDamageType);
		UpdateDiscreteStat(EAutobattlerDiscreteType::DamageType, DamageTypeText != nullptr ? *DamageTypeText : FText::GetEmpty(), IsPermanent);
	}
}

void AAutobattlerCharacter::UpdateDiscreteStat_Implementation(EAutobattlerDiscreteType StatToModify, const FText& NewText, bool IsPermanent)
{
	GetCharacterPanelComponent()->UpdateStatDiscrete(StatToModify, NewText, IsPermanent);
}

void AAutobattlerCharacter::OnMouseBeginOverlap_Implementation()
{
	UCharacterPanelComponent::HideActiveCharacterPanel(this);
	CharacterPanelComponent->ShowCharacterPanel(true);
	CustomDepthComponent->SetNewRenderCustomDepth(true);
}

void AAutobattlerCharacter::OnMouseEndOverlap_Implementation()
{
	CharacterPanelComponent->ShowCharacterPanel(false);
	CustomDepthComponent->SetNewRenderCustomDepth(false);
}

void AAutobattlerCharacter::OnMousePrimary_Implementation()
{
	CharacterPanelComponent->LockCharacterPanel(!CharacterPanelComponent->GetIsLocked());
	CustomDepthComponent->SetLockRenderCustomDepth(true);
}

void AAutobattlerCharacter::OnDeselect_Implementation()
{
	CharacterPanelComponent->LockCharacterPanel(false);
	CharacterPanelComponent->ShowCharacterPanel(false);
	CustomDepthComponent->SetLockRenderCustomDepth(false);
	CustomDepthComponent->SetNewRenderCustomDepth(false);
}

void AAutobattlerCharacter::UpdateHealthBar()
{
	if (UHealthBar* HealthBar = Cast<UHealthBar>(HealthBarDisplay->GetWidget()))
	{
		HealthBar->UpdateHealthValues(CurrentHealth, MaxHealth);
	}
}

void AAutobattlerCharacter::ToggleHealthBarVisibility_Implementation(bool IsVisible)
{
	HealthBarDisplay->SetHiddenInGame(!IsVisible);
}

void AAutobattlerCharacter::UpdateChargeBar()
{
	if (UChargeBar* ChargeBar = Cast<UChargeBar>(ChargeBarDisplay->GetWidget()))
	{
		ChargeBar->UpdateChargeValues(Charges, MaxCharges);
	}
}

void AAutobattlerCharacter::UpdateCurrentBudget()
{
	UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByID(this, OwnerID);
	AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this);

	if (IsValid(ControllerComponent) && IsValid(Manager))
	{
		ControllerComponent->RequestBudgetUIUpdate(EBudgetUIType::Current, Manager->GetCurrentBudgetForEntity(OwnerID));
	}
}

bool AAutobattlerCharacter::GetHealthBarInitiallyVisible()
{
	if (APlayerController* LocalPlayerController = UAutobattlerControllerComponent::GetLocalPlayerController(this))
	{
		if (UAutobattlerInGameUILogicProvider* UILogicProvider = Cast<UAutobattlerInGameUILogicProvider>(LocalPlayerController->GetComponentByClass(UAutobattlerInGameUILogicProvider::StaticClass())))
		{
			return UILogicProvider->ShouldShowInGameUIElements();
		}
	}

	return false;
}

void AAutobattlerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAutobattlerCharacter, CurrentHealth);
	DOREPLIFETIME(AAutobattlerCharacter, MaxHealth);
	DOREPLIFETIME(AAutobattlerCharacter, Charges);
	DOREPLIFETIME(AAutobattlerCharacter, MaxCharges);
	DOREPLIFETIME(AAutobattlerCharacter, ID);
	DOREPLIFETIME(AAutobattlerCharacter, BudgetCost);
	DOREPLIFETIME(AAutobattlerCharacter, OwnerID);
}

void AAutobattlerCharacter::OnRep_CurrentHealth()
{
	UpdateHealthBar();
}

void AAutobattlerCharacter::OnRep_MaxHealth()
{
	UpdateHealthBar();
}

void AAutobattlerCharacter::OnRep_GainCharges()
{
	UpdateChargeBar();
}

void AAutobattlerCharacter::OnRep_MaxCharges()
{
	UpdateChargeBar();
}

void AAutobattlerCharacter::OnRep_BudgetChanged()
{
	UpdateCurrentBudget();
}

void AAutobattlerCharacter::OnRep_OwnerID()
{
	OnOwnerIDChanged(OwnerID);
}

void AAutobattlerCharacter::ReplicateExecuteAbilityAnimations_Implementation(UAnimSequence* Animation, float ActionSpeed, EActionType ActionType)
{
	if (UAutobattlerAnimInstance* AutobattlerAnimInstance = Cast<UAutobattlerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		if (ActionType == EActionType::Attacking)
		{
			AutobattlerAnimInstance->SetAttackParams(Animation, ActionSpeed + AttackSpeedModifier);
			AutobattlerAnimInstance->ActionType = ActionType;
		}
		else if (ActionType == EActionType::UsingSkill)
		{
			AutobattlerAnimInstance->SetSkillParams(Animation, ActionSpeed + SkillSpeedModifier);
			AutobattlerAnimInstance->ActionType = ActionType;
		}
	}
}

void AAutobattlerCharacter::SetAnimationActionType_Implementation(EActionType NewActionType)
{
	if (UAutobattlerAnimInstance* AutobattlerAnimInstance = Cast<UAutobattlerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AutobattlerAnimInstance->ActionType = NewActionType;
	}
}

void AAutobattlerCharacter::OnGamePhaseChanged(EAutobattlerPhase NewGamePhase)
{
	CurrentGamePhase = NewGamePhase;
}
