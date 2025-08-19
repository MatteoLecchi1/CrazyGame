// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h" 	
#include "Blueprint/IUserObjectListEntry.h"
#include "Structs/TileDefinition.h"
#include "Components/Image.h"
#include "SkillListEntry.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY_API USkillListEntry : public UUserWidget , public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category = UI)
	int ListIndex;
	UPROPERTY(BlueprintReadWrite, Category = UI)
	FSkillDefinition Skill;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* SkillIcon;

};
