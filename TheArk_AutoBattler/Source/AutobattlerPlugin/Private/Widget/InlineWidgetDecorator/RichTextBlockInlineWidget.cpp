// Fill out your copyright notice in the Description page of Project Settings.

/* Class header. */
#include "Widget/InlineWidgetDecorator/RichTextBlockInlineWidget.h"

/* Autobattler includes. */
#include "Widget/InlineWidgetDecorator/InlineWidgetListener.h"

/* Engine includes. */
#include "Application/SlateApplicationBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"

class FRichInlineWidget : public FRichTextDecorator
{
public:
	FRichInlineWidget(URichTextBlock* InOwner, URichTextBlockInlineWidget* InDecorator)
		: FRichTextDecorator(InOwner),
        ParentTextBlock(InOwner),
		Decorator(InDecorator)
	{
	}

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		if (RunParseResult.Name == TEXT("widget") && RunParseResult.MetaData.Contains(TEXT("src")))
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("src")];
			const FString Src = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);

			const bool bWarnIfMissing = false;
			return Decorator->FindWidgetClass(*Src) != nullptr;
		}

		return false;
	}

protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override
	{
	    const FString* const SrcString = RunInfo.MetaData.Find(TEXT("src"));
	    const FString* const ArgumentString = RunInfo.MetaData.Find(TEXT("args"));
	    const FString* const ArgumentDelimitersString = RunInfo.MetaData.Find(TEXT("argdelim"));

        const FRichInlineWidgetClass* WidgetClassRow = Decorator->FindWidgetClass(SrcString != nullptr ? FName(*SrcString) : FName());
        if (WidgetClassRow != nullptr && ParentTextBlock.IsValid())
        {
            TSubclassOf<UUserWidget> UserWidgetClass = WidgetClassRow->WidgetClass;
            
            if (UserWidgetClass != nullptr)
		    {
			    UUserWidget* UserWidget = NewObject<UUserWidget>(ParentTextBlock.Get(), UserWidgetClass);
			    if (UserWidget != nullptr)
			    {
                    TArray<FString> ArgsArray;
				    if (ArgumentString != nullptr && !ArgumentString->IsEmpty() && UserWidget->GetClass()->ImplementsInterface(UInlineWidgetListener::StaticClass()))
				    {
					    FString DelimiterSequence(",");
					    if (ArgumentDelimitersString != nullptr)
					    {
						    DelimiterSequence = *ArgumentDelimitersString;
					    }

					    FString SourceString = *ArgumentString;
						SourceString.ParseIntoArray(ArgsArray, *DelimiterSequence);

						IInlineWidgetListener::Execute_OnInlineWidgetConstructed(UserWidget, ArgsArray);
				    }
					
                    return UserWidget->TakeWidget();
			    }
		    }
        }

        return TSharedPtr<SWidget>();
	}

private:
	URichTextBlockInlineWidget* Decorator;
    TWeakObjectPtr<URichTextBlock> ParentTextBlock;
};

URichTextBlockInlineWidget::URichTextBlockInlineWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> URichTextBlockInlineWidget::CreateDecorator(URichTextBlock* InOwner)
{
    return MakeShareable(new FRichInlineWidget(InOwner, this));
}

FRichInlineWidgetClass* URichTextBlockInlineWidget::FindWidgetClass(const FName& RowName)
{
    if (IsValid(ClassDatatable))
    {
        return ClassDatatable->FindRow<FRichInlineWidgetClass>(RowName, FString("Inline Widget Class"));
    }

    return nullptr;
}
