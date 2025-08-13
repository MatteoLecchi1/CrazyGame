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
	USkillListEntry* SelectedSkillItem = Cast<USkillListEntry>(SkillList->GetSelectedItem());
	if (SelectedSkillItem)
	{
		APlayerPawn* playerpawn = Cast<APlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		playerpawn->SelectedSkillIndex = SelectedSkillItem->ListIndex;
		playerpawn->SelectionState = PlayerSelectionState::SKILL;
	}
}
void UHUDWidget::UpdateAPValues(int currentAP, int maxAP) 
{
	FString newTextString = "";
	newTextString.AppendInt(currentAP);
	newTextString.Append("/");
	newTextString.AppendInt(maxAP);

	FText newText = FText::FromString(newTextString);
	APTextBox->SetText(newText);
}

void UHUDWidget::UpdateSkillListVisuals() 
{
	auto Items = SkillList->GetListItems();
	APlayerPawn* playerpawn = Cast<APlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	for (int i = 0; i < Items.Num(); i++)
	{
		if(playerpawn->SelectedSkillIndex == i)
		{
			SkillList->SetItemSelection(Items[i], true);
		}
		else
		{
			SkillList->SetItemSelection(Items[i], false);
		}
	}	
}