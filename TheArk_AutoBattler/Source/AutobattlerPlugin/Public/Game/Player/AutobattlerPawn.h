// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerPawn.generated.h"

UENUM(BlueprintType)
enum class EPossessor : uint8
{
	Client UMETA(DisplayName = "Client"),
	Server UMETA(DisplayName = "Server")
};

class AAutobattlerPreviewCharacter;
class UCameraComponent;

UCLASS()
class AUTOBATTLERPLUGIN_API AAutobattlerPawn : public APawn
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* By which player this pawn should be possessed. (Note: 0 is invalid, as 0 is the AI player). */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	EEntity WhoShouldOwn;

	/* True if we should try to possess this pawn when identities have been assigned. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autobattler")
	bool PossessOnAssignIdentity;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Cached player controller. */
	UPROPERTY()
	APlayerController* CurrentPlayerController;

	/* Last actor which implements AutobattlerMouseInterface moused over. */
	UPROPERTY()
	AActor* LastMousedOverActor;

	/* Mouse collision channel. */
	UPROPERTY(Replicated)
	TEnumAsByte<ECollisionChannel> MouseCollisionChannel;

	/* Owning entity of this pawn. */
	UPROPERTY(Replicated)
	EEntity WhoOwns;

	/* Flags for whether or not the character panel is currently shown
	and whether showing it is currently locked. */
	bool IsShowingCharacterPanel;
	bool CharacterPanelShowLocked;

	/* Marked as friend so it can set identity. */
	friend class UAutobattlerControllerComponent;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Default constructor.
	 */
	AAutobattlerPawn();

	/**
	 * Bind input.
	 * @param PlayerInputComponent Component to bind input to.
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	 * Tries to determine who is the owner.
	 * @param NewController Should be a player controller which has a AutobattlerControllerComponent.
	 */
	virtual void PossessedBy(AController* NewController) override;

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Whether or not this pawn should be possessed when controller component identities are assigned.
	 * @return Whether or not this pawn should be possessed when controller component identities are assigned. 
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetPossessOnAssignIdentity() const { return PossessOnAssignIdentity; }

	/**
	 * Gets which player should own this pawn.
	 * @return Who should own this pawn.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	EEntity GetWhoShouldOwn() const { return WhoShouldOwn; }

/////////////////////////////////////////////////////////////////////////////////
//// NETWORKING
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Network scaffolding.
	 * @param OutLifetimeProps Unused.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

/////////////////////////////////////////////////////////////////////////////////
//// GAME
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Used for grid traces.
	 */
	virtual void Tick(float DeltaTime) override;

/////////////////////////////////////////////////////////////////////////////////
//// CHARACTER PANEL
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Checks to see if mouse is overlapping a character with a CharacterPanelComponent.
	 * Shows the character panel if it is, appropriately handling whether it was client or
	 * server who requested showing the panel.
	 */
	void HandleMouseOverCharacter(bool RequestedByServer);

/////////////////////////////////////////////////////////////////////////////////
//// INPUT
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Tries to execute primary action on autobattler controller component. 
	 */
	UFUNCTION()
	void ExecutePrimaryAction();

	/**
	 * Tries to execute secondary action on autobattler controller component. 
	 */
	UFUNCTION()
	void ExecuteSecondaryAction();
};
