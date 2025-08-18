// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Core/GameplayPawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGameplayPawn::AGameplayPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameplayPawn::BeginPlay()
{
	Super::BeginPlay();
}
void AGameplayPawn::Initialize()
{
	Grid = Cast<AGridManagerActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManagerActor::StaticClass()));

	GameMode = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());

	SetActorTickEnabled(true);
}

// Called every frame
void AGameplayPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameplayPawn::SetAP(int APAmmount)
{
	CurrentAP = APAmmount;
	if (CurrentAP > MaxAP)
		CurrentAP = MaxAP;
}

void AGameplayPawn::OnTurnStart()
{
	SetAP(CurrentAP + APregen);
}

// Called to bind functionality to input
void AGameplayPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

