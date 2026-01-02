// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Debuffs/DOTDebuff.h"

void UDOTDebuff::OnBeginTurnWithDebuff_Implementation(AGridManagerActor* Grid)
{
	DebuffedCharacter->MyTakeDamage(Damage,DamageElement);
	Counter--;
	if (Counter <= 0)
	{
		DebuffedCharacter->Debuffs.Remove(this);
	}
	return;
}
void UDOTDebuff::OnEndTurnWithDebuff_Implementation(AGridManagerActor* Grid)
{
	return;
}
float UDOTDebuff::OnDebuffApplyed_Implementation(AGridManagerActor* Grid, AGameplayCharacter* InstigatingCharacter)
{
	return BaseApplicationReward;
}