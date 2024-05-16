// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/AutobattlerStructs.h"
#include "CharacterPanelActor.generated.h"

class UCharacterPanelComponent;

/**
 * This is an actor which exists only to have an attached character panel component, so it can be viewed/hidden.
 * In the instance when we want to show a character panel, but do not have an actor to attach the character panel component to
 * (e.g. we want to show a character panel based on a widget), we can use this actor. Otherwise, the character panel component
 * can be directly attached to the actor (for example, this is done for AAutobattlerCharacter).
 * 
 * @see CharacterPanelComponent.h - For details about the character panel component
 * @see CharacterPanelWidget.h - For details about the character panel widget itself.
 * 
 */
UCLASS()
class AUTOBATTLERPLUGIN_API ACharacterPanelActor : public AActor
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharacterPanelComponent* CharacterPanel;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Which character is the owner of this actor. */
	UPROPERTY(Replicated)
	int32 OwnerID;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	ACharacterPanelActor();

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
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Getter for character panel component.
	 * @return Character panel component.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	UCharacterPanelComponent* GetCharacterPanel() const { return CharacterPanel; }

	/**
	 * Getter for character owner ID.
	 * @return Character owner ID.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	int32 GetOwnerID() const { return OwnerID; }

	/**
	 * Gets the character panel actor from a given ID.
	 * @return Character owner ID.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler", meta = (WorldContext = "WorldContextObject"))
	static ACharacterPanelActor* GetCharacterPanelActorByID(const UObject* WorldContextObject, int32 ID);

	/**
	 * Attempts to generate character panel for this actor.
	 * @param CharacterPanelClass Class of character panel to generate widget from.
	 * @param NewDefinition Character definition to generate character panel.
	 * @param ID Character owner ID of this character panel.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void BuildCharacterPanelActor(TSubclassOf<UCharacterPanel> CharacterPanelClass, const FAutobattlerCharacterDefinition& NewDefinition, int32 NewID);
};
