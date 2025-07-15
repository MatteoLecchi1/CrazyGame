// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Core/CrazyGameInstance.h"


UCrazyGameInstance* UCrazyGameInstance::GetGameInstance(const UObject* WorldContextObject)
{
	return Cast<UCrazyGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

FSkillDefinition UCrazyGameInstance::GetSkillFromKey(FName SkillKey)
{
	FSkillDefinition Skill;
	SkillsDataTable->ForeachRow<FSkillDefinition>("Skill", [&](const FName& Key, const FSkillDefinition& SkillDefinition) {
		
		if(Key == SkillKey)
			Skill = SkillDefinition;
	});

	return Skill;
}