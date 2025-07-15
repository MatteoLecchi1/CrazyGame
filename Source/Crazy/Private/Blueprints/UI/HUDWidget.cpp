// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/UI/HUDWidget.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprints/Player/PlayerPawn.h"

void UHUDWidget::UpdateSkillList(TArray<FSkillDefinition> Skills)
{
	SkillList->ClearListItems();
	for (int i = 0; i < Skills.Num(); i++)
	{
		USkillListEntry* Item = NewObject<USkillListEntry>();
		Item->ListIndex = i;
		SkillList->AddItem(Item);
	}
}
void UHUDWidget::UpdateSelectedSkill()
{
	APlayerPawn* playerpawn = Cast<APlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	USkillListEntry* SelectedSkillItem = Cast<USkillListEntry>(SkillList->GetSelectedItem());
	if (SelectedSkillItem)
	{
		playerpawn->SelectedSkillIndex = SelectedSkillItem->ListIndex;
	}
}
