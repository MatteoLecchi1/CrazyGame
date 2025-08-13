// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs/TileDefinition.h"
#include "Blueprints/Grid/GridManagerActor.h"
#include "Blueprints/UI/SkillListEntry.h"
#include "Components/TextBlock.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UListView* SkillList;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* APTextBox;
	UFUNCTION()
	void UpdateSkillList(TArray<FSkillDefinition> Skills);
	UFUNCTION()
	void UpdateSkillListVisuals();
	UFUNCTION()
	void UpdateAPValues(int currentAP, int maxAP);

	UFUNCTION(BlueprintCallable)
	void UpdateSelectedSkill();
};
