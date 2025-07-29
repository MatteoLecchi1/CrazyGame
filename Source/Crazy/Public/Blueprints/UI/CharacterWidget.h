// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "CharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY_API UCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* TextBox;
	

	void UpdateHPValues(int currentHP, int maxHP);

};
