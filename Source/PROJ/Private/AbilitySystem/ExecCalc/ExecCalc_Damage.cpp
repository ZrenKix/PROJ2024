// © 2024 Choklad4Life. All rights reserved.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "DBGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/DBAbilitySystemLibrary.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct DBDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DBDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UProjAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UProjAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UProjAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UProjAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UProjAttributeSet, CriticalHitDamage, Source, false);
	}
};

static const DBDamageStatics& DamageStatics()
{
	static DBDamageStatics DBDamageStatics;
	return DBDamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DBDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DBDamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DBDamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DBDamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DBDamageStatics().CriticalHitDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{

	// Get the Source and Target attributes
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// Get the Source and Target actors
	AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	
	//Casting to combat interface to get level of character for later use
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceActor);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetActor);

	// Get owning spec
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Get the Source and Target tags
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Get damage set by caller magnitued
	 float Damage = 0.f;
	 for(FGameplayTag DamageTypeTag : FDBGameplayTags::Get().DamageTypes)
	 {
	 	const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag);
	 	
	 	const float MaxDamageTypeValue = DamageTypeValue * 1.5f;
	 	
	 	const float RandomizedDamage = FMath::RandRange(DamageTypeValue, MaxDamageTypeValue);
	 	
	 	Damage += RandomizedDamage;
	 }

	if (SourceASC && SourceASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Healer.DMGBuff"))))
	{
		UE_LOG(LogTemp, Warning, TEXT("Source ASC has DMGBuff gameplay tag! Adding bonus damage."));
		Damage += 5.f;

		SourceASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Healer.DMGBuff")));
		UE_LOG(LogTemp, Warning, TEXT("DMGBuff gameplay tag removed from Source ASC."));
	}
	

	// Capture blockchance on target, and determine if there was a successful block
	// If there was a successful block, reduce the damage by half
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DBDamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max(TargetBlockChance, 0.f);

	const bool bBlocked = FMath::RandRange(1, 100) <= TargetBlockChance;
	Damage = bBlocked ? Damage / 2.f : Damage;

	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

	UCharacterClassInfo* CharacterClassInfo = UDBAbilitySystemLibrary::GetCharacterClassInfo(SourceActor);

	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(1.f);

	//Critical Hit Resistance reduces critical hit chance by a certain percentage
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) <= EffectiveCriticalHitChance;
	

	//Double damage plus a bonus if critical hit
	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;

	// Ensure Damage is an integer value
	int32 RoundedDamage = FMath::RoundToInt(Damage);
	
	const FGameplayModifierEvaluatedData EvaluatedData(UProjAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, static_cast<float>(RoundedDamage));
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
