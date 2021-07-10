// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueResponse.generated.h"

/* Individual responses are stored in this widget. */
UCLASS()
class TOPDOWNDIALOGUES_API UDialogueResponse : public UUserWidget
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* A button which, when clicked, confirms a response. */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ResponseButton;

	/* The text of a response. */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ResponseText;

private:
	/* Mark that UDialogueBox is a friend class. */
	friend class UDialogueBox;

	/* Internal use variables to pass back to UDialogueBox. */
	class UDialogueBox* OwnerDialogueBox;

	/* Internal use storage for row name. */
	FName RowNameResponse;

/* --- FUNCTIONS --- */
protected:
	/* Native construct to bind button delegate. */
	virtual void NativeConstruct() override;

/* --- FUNCTIONS --- */
private:
	/**
	 * This is intended to be called by UDialogueBox to properly initialise this widget.
	 * @param OwningDialogueBox The dialogue box which this widget is "owned" by.
	 * @param ResponseRownName The provided response row name.
	 * @param ResponseText Actual text content of the response. 
	 */
	void InitialiseResponse(class UDialogueBox* OwningDialogueBox, const FName& ResponseDatatableRowName, const FString& ResponseTextContent);

	/**
	 * Function bound to ResponseButton. Advances the dialogue. 
	 */
	UFUNCTION()
	void ResponseButtonClicked();
	
};
