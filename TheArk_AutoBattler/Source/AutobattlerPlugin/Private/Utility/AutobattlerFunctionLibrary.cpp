// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

/* Class header. */
#include "Utility/AutobattlerFunctionLibrary.h"

/* Autobattler includes. */
#include "Core/AutobattlerManager.h"
#include "DataAssets/AutobattlerConfiguration.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "UI/Deployment/CharacterPanel.h"

/* Engine includes. */
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"

DEFINE_LOG_CATEGORY(LogAutobattler);

void UAutobattlerFunctionLibrary::MakeDebugEntityConfigurationNoModifiers(FEntityConfiguration& EntityConfiguration, int32 MaxCharactersToDeploy, const FIntPair& DeploymentRangeMin, const FIntPair& DeploymentRangeMax, const TArray<FName>& CharacterRowNames)
{
    FEntityConfiguration OutEntityConfiguration;
    OutEntityConfiguration.MaxCharactersToDeploy = MaxCharactersToDeploy;

    TArray<FIntPair> Pairs;
    for (int32 x = DeploymentRangeMin.X; x <= DeploymentRangeMax.X; x++)
    {
        for (int32 y = DeploymentRangeMin.Y; y <= DeploymentRangeMax.Y; y++)
        {
            Pairs.Emplace(FIntPair(x, y));
        }
    }

    OutEntityConfiguration.ValidGridDeploymentLocations = Pairs;

    for (auto CharacterRowName : CharacterRowNames)
    {
        OutEntityConfiguration.AvailableCharacters.Emplace(FCharacterListing(CharacterRowName));
    }

    EntityConfiguration = OutEntityConfiguration;
}

void UAutobattlerFunctionLibrary::ClearCharacterPanelsInViewport(const UObject* WorldContextObject, bool Force, int32 IDToRemove)
{
    if (GEngine == nullptr) return;

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!IsValid(World)) return;

    for(TObjectIterator<UCharacterPanel> Itr; Itr; ++Itr)
    {
        UCharacterPanel* Panel = *Itr;
        if (!IsValid(Panel) || Panel->GetWorld() != World || Panel->IsInViewport()) continue;
        if (Force || Panel->CharacterPanelID == IDToRemove) Panel->RemoveFromViewport();
    }
}

void UAutobattlerFunctionLibrary::ClearInvalidCharacterPanels(const UObject* WorldContextObject)
{
    if (GEngine == nullptr) return;

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!IsValid(World)) return;

    TArray<int32> CharacterIDs;
    for (TActorIterator<AAutobattlerCharacter> Itr(World); Itr; ++Itr)
    {
        AAutobattlerCharacter* Character = *Itr;
        if (!IsValid(Character)) continue;

        CharacterIDs.Emplace(Character->GetID());
    }

    for(TObjectIterator<UCharacterPanel> Itr; Itr; ++Itr)
    {
        UCharacterPanel* Panel = *Itr;
        if (!IsValid(Panel) || Panel->GetWorld() != World) continue;
        if (!CharacterIDs.Contains(Panel->CharacterPanelID)) Panel->RemoveFromViewport();
    }
}

void UAutobattlerFunctionLibrary::ChangeInGameUIVisibility(const UObject* WorldContextObject, bool ShouldBeVisible)
{
    if (GEngine == nullptr) return;

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!IsValid(World)) return;

    for (TActorIterator<AAutobattlerCharacter> Itr(World); Itr; ++Itr)
    {
        AAutobattlerCharacter* Character = *Itr;
        if (!IsValid(Character)) continue;

        Character->ToggleUIElementsLocal(ShouldBeVisible);
    }
}

FAutobattlerCharacterDefinition* UAutobattlerFunctionLibrary::GetCharacterDefinitionFromConfigurationDatatable(const UObject* WorldContextObject, const FName& RowName)
{
    const UAutobattlerConfiguration* ConfigurationAsset = UAutobattlerConfiguration::GetConfigurationAsset(WorldContextObject);
	if (ConfigurationAsset == nullptr)
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString("AutobattlerFunctionLibrary : [InitialiseCharacterListings] Could not get configuration asset!"));
		return nullptr;
	}

    const UDataTable* CharactersDT = ConfigurationAsset->AllCharactersDataTable;
	if (CharactersDT->GetRowNames().Num() < 1)
	{
        #if WITH_EDITOR
		PrintErrorToLog(
			FString::Printf(TEXT("AutobattlerFunctionLibrary : [InitialiseCharacterListings] Character DT should have row structure AutobattlerCharacterDefinition but instead has %s and has %d rows!"), 
			*CharactersDT->GetRowStructName().ToString(),
			CharactersDT->GetRowNames().Num()
			)
		);
        #endif
		return nullptr;
	}

    return CharactersDT->FindRow<FAutobattlerCharacterDefinition>(RowName, FString("Autobattler Character Definition"));
}

UBlackboardComponent* UAutobattlerFunctionLibrary::GetPawnBlackboardComponent(const APawn* PawnOwner)
{
    if (!IsValid(PawnOwner)) return nullptr;

    if (AAIController* AIController = Cast<AAIController>(PawnOwner->GetController()))
	{
        return AIController->GetBlackboardComponent();
	}
	else
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(
		FString::Printf(TEXT("%s : [GetPawnBlackboardComponent] Does not have or is not possessed by an AI Controller!"),
			*PawnOwner->GetName()
		));
	}

    return nullptr;
}

APawn* UAutobattlerFunctionLibrary::GetBehaviourTreePawnOwner(UBehaviorTreeComponent& OwnerComp)
{
    if (AAIController* AIOwner = OwnerComp.GetAIOwner())
    {
        return AIOwner->GetPawn();
    }

    return nullptr;
}

bool UAutobattlerFunctionLibrary::IsEnemy(EEntity OwnerOne, EEntity OwnerTwo)
{
    if (OwnerOne != EEntity::AI) return OwnerTwo == EEntity::AI;
    else return OwnerTwo != EEntity::AI;
}

void UAutobattlerFunctionLibrary::GetCharactersFiltered(TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter* ContextCharacter, EAbilityFilterType FilterType, bool FilterDead)
{
    if (!IsValid(ContextCharacter) || !IsValid(ContextCharacter->GetWorld())) 
    {
        FilteredCharacters.Empty();
        return;
    }

    TArray<AAutobattlerCharacter*> CharactersToChooseFrom;
    for (TActorIterator<AAutobattlerCharacter> ActorItr(ContextCharacter->GetWorld()); ActorItr; ++ActorItr)
    {
        AAutobattlerCharacter* CurrentCharacter = *ActorItr;
        if (!IsValid(CurrentCharacter)) continue;
        CharactersToChooseFrom.Emplace(CurrentCharacter);
    }

    CharacterFilterAlgorithm(FilteredCharacters, CharactersToChooseFrom, ContextCharacter, FilterType, FilterDead);
}

void UAutobattlerFunctionLibrary::GetCharactersFilteredInBox(TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter* ContextCharacter, TEnumAsByte<ECollisionChannel> CharacterCollisionObjectType, const FVector& BoxOrigin, const FVector& BoxExtent, EAbilityFilterType FilterType, bool FilterDead)
{
    if (!IsValid(ContextCharacter) || !IsValid(ContextCharacter->GetWorld())) 
    {
        FilteredCharacters.Empty();
        return;
    }

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(CharacterCollisionObjectType.GetValue());

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(ContextCharacter);
    CollisionQueryParams.bTraceComplex = false;

    TArray<FHitResult> HitResults;
    ContextCharacter->GetWorld()->SweepMultiByObjectType(
        HitResults, 
        BoxOrigin, 
        BoxOrigin,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeBox(BoxExtent),
        CollisionQueryParams
    );

    TArray<AAutobattlerCharacter*> CharactersToChooseFrom;
    for (auto HitResult : HitResults)
    {
        if (AAutobattlerCharacter* QueryCharacter = Cast<AAutobattlerCharacter>(HitResult.GetActor()))
        {
            CharactersToChooseFrom.Emplace(QueryCharacter);
        }
    }

    CharacterFilterAlgorithm(FilteredCharacters, CharactersToChooseFrom, ContextCharacter, FilterType, FilterDead);
}

void UAutobattlerFunctionLibrary::GetCharactersFilteredInSphere(TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter* ContextCharacter, TEnumAsByte<ECollisionChannel> CharacterCollisionObjectType, const FVector& SphereOrigin, float SphereRadius, EAbilityFilterType FilterType, bool FilterDead)
{
    if (!IsValid(ContextCharacter) || !IsValid(ContextCharacter->GetWorld())) 
    {
        FilteredCharacters.Empty();
        return;
    }

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(CharacterCollisionObjectType.GetValue());

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(ContextCharacter);
    CollisionQueryParams.bTraceComplex = false;

    TArray<FHitResult> HitResults;
    ContextCharacter->GetWorld()->SweepMultiByObjectType(
        HitResults, 
        SphereOrigin, 
        SphereOrigin,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(SphereRadius),
        CollisionQueryParams
    );

    TArray<AAutobattlerCharacter*> CharactersToChooseFrom;
    for (auto HitResult : HitResults)
    {
        if (AAutobattlerCharacter* QueryCharacter = Cast<AAutobattlerCharacter>(HitResult.GetActor()))
        {
            CharactersToChooseFrom.Emplace(QueryCharacter);
        }
    }

    CharacterFilterAlgorithm(FilteredCharacters, CharactersToChooseFrom, ContextCharacter, FilterType, FilterDead);
}

void UAutobattlerFunctionLibrary::CharacterFilterAlgorithm(TArray<AAutobattlerCharacter*>& FilteredCharacters, const TArray<AAutobattlerCharacter*>& CharacterToChooseFrom, AAutobattlerCharacter* ContextCharacter, EAbilityFilterType FilterType, bool FilterDead)
{
    FilteredCharacters.Empty();
    if (!IsValid(ContextCharacter))
    {
        PrintErrorToLog(FString("Autobattler Library : [CharacterFilterAlgorithm] ContextCharacter is invalid!"));
        return;
    }

    EEntity ContextCharacterOwner = ContextCharacter->GetOwnerIdentity();

    if (FilterType == EAbilityFilterType::Self || FilterType == EAbilityFilterType::SelfAndAllies)
    {
        FilteredCharacters.AddUnique(ContextCharacter);
        return;
    }

    for (auto CurrentCharacter : CharacterToChooseFrom)
    {
        if (!IsValid(CurrentCharacter)) continue;
        if (FilterDead && CurrentCharacter->GetIsDead()) continue;

        EEntity CurrentCharacterOwner = CurrentCharacter->GetOwnerIdentity();

        if (FilterType == EAbilityFilterType::All)
        {
            FilteredCharacters.AddUnique(CurrentCharacter);
        }
        else if (FilterType == EAbilityFilterType::SelfAndAllies || FilterType == EAbilityFilterType::AlliesOnly)
        {
            if (ContextCharacter == CurrentCharacter) continue;
            if (!IsEnemy(ContextCharacterOwner, CurrentCharacterOwner)) FilteredCharacters.AddUnique(CurrentCharacter);
        }
        else if (FilterType == EAbilityFilterType::Enemies)
        {
            if (ContextCharacter == CurrentCharacter) continue;
            if (IsEnemy(ContextCharacterOwner, CurrentCharacterOwner)) FilteredCharacters.AddUnique(CurrentCharacter);
        }
    }
}

int32 UAutobattlerFunctionLibrary::ManhattanDistance(const FIntPair& A, const FIntPair& B)
{
    return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y);
}

int32 UAutobattlerFunctionLibrary::VManhattanDistance(const FVector& A, const FVector& B)
{
    return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y) + FMath::Abs(A.Z - B.Z);
}

float UAutobattlerFunctionLibrary::CartesianDistance(const FVector& A, const FVector& B)
{
    return FMath::Sqrt(FMath::Square(B.X - A.X) + FMath::Square(B.Y - A.Y) + FMath::Square(B.Z - A.Z));
}

FCharacterBaseStats UAutobattlerFunctionLibrary::MakeBaseStatsFromDefinition(const UObject* WorldContextObject, const FAutobattlerCharacterDefinition& Definition)
{
    FCharacterBaseStats OutStats;
    OutStats.BaseCriticalChance = Definition.CriticalChance;
    OutStats.BaseCriticalMultiplier = Definition.CriticalMultiplier;
    OutStats.BaseHealth = Definition.BaseHealth;
    OutStats.BaseMovementSpeed = Definition.MovementSpeed;

    if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(WorldContextObject))
    {
        if (const UAutobattlerConfiguration* Configuration = Manager->GetAutobattlerConfigurationAsset())
        {
            if (const FText* DamageText = Configuration->DamageTextMap.Find(Definition.BaseDamageType)) OutStats.BaseDamageType = *DamageText;
            else OutStats.BaseDamageType = FText::GetEmpty();

            if (const FText* ResistanceText = Configuration->ResistanceTextMap.Find(Definition.BaseResistanceType)) OutStats.BaseResistanceType = *ResistanceText;
            else OutStats.BaseResistanceType = FText::GetEmpty();
        }
    }
    else
    {
        OutStats.BaseResistanceType = FText::GetEmpty();
        OutStats.BaseDamageType = FText::GetEmpty();
    }

    return OutStats;
}

void UAutobattlerFunctionLibrary::ParseTextIntoStringArray(const FText& SourceText, const FString& Delimiter, TArray<FName>& Elements)
{
    FString SourceString = SourceText.ToString();
    TArray<FString> StringArray;
    SourceString.ParseIntoArray(StringArray, *Delimiter);
    Elements.Empty(StringArray.Num());

    for (auto String : StringArray)
    {
        String.TrimStartAndEndInline();
        Elements.Emplace(String);
    }
}

void UAutobattlerFunctionLibrary::PrintMessage(const FString& Message, EVerbosityLevel VerbosityLevel, bool ToScreen, bool ToLog, int32 Key, float Time)
{
    if (GEngine != nullptr && ToScreen)
    {
        if (VerbosityLevel == EVerbosityLevel::Log) GEngine->AddOnScreenDebugMessage(Key, Time, FColor::Cyan, Message);
        else if (VerbosityLevel == EVerbosityLevel::Warning) GEngine->AddOnScreenDebugMessage(Key, Time, FColor::Yellow, Message);
        else if (VerbosityLevel == EVerbosityLevel::Error) GEngine->AddOnScreenDebugMessage(Key, Time, FColor::Red, Message);
    }

    if (ToLog)
    {
        if (VerbosityLevel == EVerbosityLevel::Log) UE_LOG(LogAutobattler, Log, TEXT("%s"), *Message);
        if (VerbosityLevel == EVerbosityLevel::Warning) UE_LOG(LogAutobattler, Warning, TEXT("%s"), *Message);
        if (VerbosityLevel == EVerbosityLevel::Error) UE_LOG(LogAutobattler, Error, TEXT("%s"), *Message);
    }
}

void UAutobattlerFunctionLibrary::PrintErrorToLog(const FString& Message)
{
    PrintMessage(Message, EVerbosityLevel::Error, false, true);
}

void UAutobattlerFunctionLibrary::PrintWarningToLog(const FString& Message)
{
    PrintMessage(Message, EVerbosityLevel::Warning, false, true);
}

void UAutobattlerFunctionLibrary::PrintMessageToLog(const FString& Message)
{
    PrintMessage(Message, EVerbosityLevel::Log, false, true);
}
