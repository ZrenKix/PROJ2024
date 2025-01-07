// © 2024 Choklad4Life. All rights reserved.

#include "Arena/Unit/HeroUnit.h"

#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "Arena/ArenaManager.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PROJ/PROJ.h"

AHeroUnit::AHeroUnit()
{

	AbilitySystemComponent = CreateDefaultSubobject<UProjAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Kolla https://imgur.com/a/qExGmrP för mer info


	AttributeSet = CreateDefaultSubobject<UProjAttributeSet>(TEXT("AttributeSet"));

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	DMGBuffEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DMGBuffEffect"));
	DMGBuffEffect->SetupAttachment(RootComponent);
	DMGBuffEffect->bAutoActivate = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	
}

void AHeroUnit::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
	AddDefaultAbilities();
}

bool AHeroUnit::ActionTurn()
{	
	OnActionTurn.Broadcast();
	return true;
}

int AHeroUnit::GainXp(int Amount)
{
	return Super::GainXp(Amount);
}

bool AHeroUnit::LevelUp()
{
	return Super::LevelUp();
}

void AHeroUnit::OnDeath()
{
	Super::OnDeath();
}

void AHeroUnit::OnPlayerTargeted_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_GREEN);
}

void AHeroUnit::OnPlayerTargetedEnd_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
}

void AHeroUnit::RemoveDMGBuff()
{
	if(DMGBuffEffect && DMGBuffEffect->IsActive())
	{
		DMGBuffEffect->Deactivate();
		DMGBuffEffect->DestroyComponent();
	}
}

void AHeroUnit::StartDMGBuffEffect()
{
	if(DMGBuffEffect)
	{
		DMGBuffEffect->Activate(true);
	}
}

void AHeroUnit::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UProjAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

bool AHeroUnit::IsDead() const
{
	if (AttributeSet)
	{
		return Cast<UProjAttributeSet>(AttributeSet)->GetHealth() <= 0;
	}
	UE_LOG(LogTemp, Error, TEXT("%s: IsDead() failed to get attribute set"), *GetName());
	return false;
}

int32 AHeroUnit::GetPlayerLevel()
{
	//const AProjPlayerState* ProjPlayerState = GetPlayerState<AProjPlayerState>();
	//check(ProjPlayerState);
	//return ProjPlayerState->GetPlayerLevel();
	return 1;
}

FVector AHeroUnit::GetCombatSocketLocation()
{
	return GetMesh()->GetSocketLocation(WeaponTipSocketName);
}

void AHeroUnit::ToggleEnemyWidget()
{
	APlayerController* PC = GetController<APlayerController>();
	if (!PC) return;

	if (bIsSettingsMenuOpen)
	{
		// Menyn är öppen, stäng den
		if (EnemySettingsWidget && EnemySettingsWidget->IsInViewport())
		{
			EnemySettingsWidget->RemoveFromParent();
		}
		// Sätt input mode till Game Only
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		//PC->bShowMouseCursor = false;
		bIsSettingsMenuOpen = false;
	}
	else
	{
		// Menyn är stängd, öppna den
		if (!EnemySettingsWidget && EnemySettingsWidgetClass)
		{
			EnemySettingsWidget = CreateWidget<UUserWidget>(PC, EnemySettingsWidgetClass);
		}

		if (EnemySettingsWidget && !EnemySettingsWidget->IsInViewport())
		{
			EnemySettingsWidget->AddToViewport();
		}
        
		// Sätt input mode till UI Only
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(EnemySettingsWidget->TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
		bIsSettingsMenuOpen = true;
	}
	UE_LOG(LogTemp, Warning, TEXT("ToggleEnemyWidget!"));
}


