// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Animation/AutobattlerAnimInstance.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"

void UAutobattlerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (APawn* PawnOwner = TryGetPawnOwner())
    {
        MovementSpeed = PawnOwner->GetVelocity().Size();
    }
}

void UAutobattlerAnimInstance::SetActionType(EActionType NewActionType)
{
    if (AAutobattlerCharacter* OwningCharacter = Cast<AAutobattlerCharacter>(TryGetPawnOwner()))
    {
        OwningCharacter->SetAnimationActionType(NewActionType);
    }
}

void UAutobattlerAnimInstance::ResetAIState()
{
    if (AAutobattlerCharacter* OwningCharacter = Cast<AAutobattlerCharacter>(TryGetPawnOwner()))
    {
        if (OwningCharacter->HasAuthority())
        {
            if (AAutobattlerAIController* AIController = Cast<AAutobattlerAIController>(OwningCharacter->GetController()))
            {
                AIController->ResetState();
            }
        }
    }
}

void UAutobattlerAnimInstance::SetSkillParams(UAnimationAsset* NewSkillAnimation, float NewSkillSpeed)
{
    SkillAnimation = NewSkillAnimation;
    SkillActionSpeed = NewSkillSpeed;
}

void UAutobattlerAnimInstance::SetAttackParams(UAnimationAsset* NewAttackAnimation, float NewAttackSpeed)
{
    AttackAnimation = NewAttackAnimation;
    AttackActionSpeed = NewAttackSpeed;
}
