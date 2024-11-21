// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ProjEnemy.h"

#include "AbilitySystem/DBAbilitySystemLibrary.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "BSTurnBasedCombat/BaseCharacter.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PROJ/PROJ.h"
#include "UI/Widget/DBUserWidget.h"
#include "DBGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

AProjEnemy::AProjEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UProjAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Kolla https://imgur.com/a/qExGmrP för mer info


	AttributeSet = CreateDefaultSubobject<UProjAttributeSet>(TEXT("AttributeSet"));

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());
	
}

void AProjEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	UDBAbilitySystemLibrary::GiveStartupAbilites(this, AbilitySystemComponent);

	if(UDBUserWidget* DBUserWidget = Cast<UDBUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		DBUserWidget->SetWidgetController(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No widget found"));
	}

	if(const UProjAttributeSet* AS = CastChecked<UProjAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
				[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
				[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(FDBGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AProjEnemy::HitReactTagChanged
		);
		
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}

}

void AProjEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void AProjEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UProjAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

void AProjEnemy::InitializeDefaultAttributes() const
{
	UDBAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AProjEnemy::OnTargeted_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AProjEnemy::OnTargetedEnd_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
}

int32 AProjEnemy::GetPlayerLevel()
{
	return Level;
}

void AProjEnemy::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}


//----------------------------------------------------
// TILLFÄLLIGT

void AProjEnemy::Attack(ABaseCharacter* Target)
{
	if (!Target)
	{
		// Get all player characters
		TArray<AActor*> PlayerCharacters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), PlayerCharacters);

		// Remove dead characters from the list
		PlayerCharacters = PlayerCharacters.FilterByPredicate([](AActor* Actor)
		{
			ABaseCharacter* Character = Cast<ABaseCharacter>(Actor);
			return Character && Character->IsAlive();
		});

		if (PlayerCharacters.Num() > 0)
		{
			// Choose a random target
			int32 RandomIndex = FMath::RandRange(0, PlayerCharacters.Num() - 1);
			Target = Cast<ABaseCharacter>(PlayerCharacters[RandomIndex]);
		}
		else
		{
			// No alive player characters left
			return;
		}
	}

	if (Target && Target->IsAlive())
	{
		// Define the damage amount
		float DamageAmount = 10.0f; // Adjust as needed

		// Apply damage to the target
		Target->Health -= DamageAmount;

		// Clamp the health to a minimum of zero
		Target->Health = FMath::Max(Target->Health, 0.0f);

		UE_LOG(LogTemp, Log, TEXT("%s attacked %s for %f damage. %s's health is now %f."),
			*GetName(), *Target->GetName(), DamageAmount, *Target->GetName(), Target->Health);

		// Check if the target has died
		if (!Target->IsAlive())
		{
			// Handle the target's death
			Target->Die();
		}
	}
}

void AProjEnemy::NotifyPlayerOfDeath()
{
	// Display a message to the player
	if (GEngine)
	{
		FString DeathMessage = FString::Printf(TEXT("%s has been defeated!"), *GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DeathMessage);
	}

	// Optionally, you can implement more sophisticated UI feedback here
}

bool AProjEnemy::IsAlive() const
{
	return Health > 0.0f;
}

//------------------------------------------------
