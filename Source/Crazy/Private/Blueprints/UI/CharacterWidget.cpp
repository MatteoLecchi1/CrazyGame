// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/UI/CharacterWidget.h"

void UCharacterWidget::UpdateHPValues(int currentHP, int maxHP)
{
	FString newTextString = "";
	newTextString.AppendInt(currentHP);
	newTextString.Append("/");
	newTextString.AppendInt(maxHP);

	FText newText = FText::FromString(newTextString);
	TextBox->SetText(newText);
}