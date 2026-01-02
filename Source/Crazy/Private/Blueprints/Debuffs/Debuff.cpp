// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Debuffs/Debuff.h"

void UDebuff::OnBeginTurnWithDebuff_Implementation(AGridManagerActor* Grid)
{
	return;
}
void UDebuff::OnEndTurnWithDebuff_Implementation(AGridManagerActor* Grid)
{
	return;
}
float UDebuff::OnDebuffApplyed_Implementation(AGridManagerActor* Grid, AGameplayCharacter* InstigatingCharacter)
{
	return BaseReward;
}