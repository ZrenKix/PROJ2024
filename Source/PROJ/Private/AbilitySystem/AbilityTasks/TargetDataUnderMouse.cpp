#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
    // Check if OwningAbility is valid before proceeding
    if (!OwningAbility)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateTargetDataUnderMouse: OwningAbility is null."));
        return nullptr;
    }

    UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
    MyObj->Ability = OwningAbility; // Set the ability
    MyObj->AbilitySystemComponent = OwningAbility->GetAbilitySystemComponentFromActorInfo(); // Initialize ASC
    return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
    if (!Ability || !AbilitySystemComponent.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Activate: Ability or AbilitySystemComponent is invalid."));
        EndTask();
        return;
    }

    const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    if (bIsLocallyControlled)
    {
        SendMouseCursorData();
    }
    else
    {
        const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
        const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
        if (!AbilitySystemComponent->IsValidLowLevel())
        {
            UE_LOG(LogTemp, Error, TEXT("Activate: AbilitySystemComponent is invalid."));
            EndTask();
            return;
        }

        AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
        const bool bCalledDelegate = AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
        if (!bCalledDelegate)
        {
            SetWaitingOnRemotePlayerData();
        }
    }
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
    if (!Ability || !AbilitySystemComponent.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("SendMouseCursorData: Ability or AbilitySystemComponent is invalid."));
        return;
    }

    FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get());

    const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
    if (!ActorInfo || !ActorInfo->PlayerController.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("SendMouseCursorData: ActorInfo or PlayerController is null."));
        return;
    }

    APlayerController* PC = ActorInfo->PlayerController.Get();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("SendMouseCursorData: PlayerController is null."));
        return;
    }

    FHitResult CursorHit;
    if (PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit))
    {
        FGameplayAbilityTargetDataHandle DataHandle;
        FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
        Data->HitResult = CursorHit;
        DataHandle.Add(Data);

        AbilitySystemComponent->ServerSetReplicatedTargetData(
            GetAbilitySpecHandle(),
            GetActivationPredictionKey(),
            DataHandle, FGameplayTag(),
            AbilitySystemComponent->ScopedPredictionKey);

        if (ShouldBroadcastAbilityTaskDelegates())
        {
            ValidData.Broadcast(DataHandle);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SendMouseCursorData: No valid hit under cursor."));
    }
}


void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
    if (!AbilitySystemComponent.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("OnTargetDataReplicatedCallback: AbilitySystemComponent is invalid."));
        return;
    }

    AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
}
