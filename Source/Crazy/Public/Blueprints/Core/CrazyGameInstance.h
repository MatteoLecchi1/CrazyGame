// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Structs/TileDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "CrazyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY_API UCrazyGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UDataTable* SkillsDataTable;

public:
	FSkillDefinition GetSkillFromKey(FName SkillKey);

	static UCrazyGameInstance* GetGameInstance(const UObject* WorldContextObject);

};
