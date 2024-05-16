// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Player/AutobattlerPawn.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"
#include "Core/AutobattlerSettings.h"
#include "DataAssets/AutobattlerConfiguration.h"
#include "Game/Components/CharacterPanelComponent.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Game/Units/AutobattlerPreviewCharacter.h"
#include "Interfaces/AutobattlerMouseInterface.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

AAutobattlerPawn::AAutobattlerPawn()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	RootComponent = Camera;

	PrimaryActorTick.bCanEverTick = true;
	
	MouseCollisionChannel = ECollisionChannel::ECC_Visibility;
}

void AAutobattlerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (IsValid(PlayerInputComponent))
	{
		if (const UAutobattlerSettings* AutobattlerSettings = GetDefault<UAutobattlerSettings>())
		{
			PlayerInputComponent->BindAction(
				AutobattlerSettings->PrimaryActionInputName,
				EInputEvent::IE_Pressed,
				this,
				&AAutobattlerPawn::ExecutePrimaryAction
			);

			PlayerInputComponent->BindAction(
				AutobattlerSettings->SecondaryActionInputName,
				EInputEvent::IE_Pressed,
				this,
				&AAutobattlerPawn::ExecuteSecondaryAction
			);
		}
		else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [SetupPlayerInputComponent] Could not get autobattler settings!"), *GetName()));
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [SetupPlayerInputComponent] Does not have a valid input component!"), *GetName()));
}

void AAutobattlerPawn::PossessedBy(AController* NewController)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		TArray<UAutobattlerControllerComponent*> ControllerComponents;
		PlayerController->GetComponents<UAutobattlerControllerComponent>(ControllerComponents);

		if (ControllerComponents.IsValidIndex(0))
		{
			WhoOwns = ControllerComponents[0]->GetIdentity();
		}
		else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [PossessedBy] Possessing controller did not have an autobattler controller component!"), *GetName()));
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [PossessedBy] Was not possessed by player controller!"), *GetName()));
}

void AAutobattlerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAutobattlerPawn, WhoOwns);
}

void AAutobattlerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleMouseOverCharacter(HasAuthority());
}

void AAutobattlerPawn::HandleMouseOverCharacter(bool RequestedByServer)
{
	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		FHitResult HitResult;
		bool Hit = PlayerController->GetHitResultUnderCursor(
			MouseCollisionChannel,
			false,
			HitResult
		);

		if (!Hit)
		{
			LastMousedOverActor = nullptr;
			return;
		}

		if (IsValid(HitResult.GetActor()))
		{
			if (HitResult.GetActor() == LastMousedOverActor) return;
			if (HitResult.GetActor()->GetClass()->ImplementsInterface(UAutobattlerMouseInterface::StaticClass()))
			{
				if (IsValid(LastMousedOverActor)) IAutobattlerMouseInterface::Execute_OnMouseEndOverlap(LastMousedOverActor);
				IAutobattlerMouseInterface::Execute_OnMouseBeginOverlap(HitResult.GetActor());
				LastMousedOverActor = HitResult.GetActor();
			}
			else
			{
				if (IsValid(LastMousedOverActor))
				{
					IAutobattlerMouseInterface::Execute_OnMouseEndOverlap(LastMousedOverActor);	
					LastMousedOverActor = nullptr;
				}
			}
		}
	}
}

void AAutobattlerPawn::ExecutePrimaryAction()
{
	if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByID(this, WhoOwns))
	{
		ControllerComponent->ExecutePrimaryAction();
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [ExecutePrimaryAction] Failed to find controller component for owner %d!"), *GetName(), *UEnum::GetValueAsString(WhoOwns)));
}

void AAutobattlerPawn::ExecuteSecondaryAction()
{
	if (UAutobattlerControllerComponent* ControllerComponent = UAutobattlerControllerComponent::GetControllerComponentByID(this, WhoOwns))
	{
		ControllerComponent->ExecuteSecondaryAction();
	}
	else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [ExecuteSecondaryAction] Failed to find controller component for owner %d!"), *GetName(), *UEnum::GetValueAsString(WhoOwns)));
}
