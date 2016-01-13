// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector.h"
#include "BatteryCollectorGameMode.h"
#include "BatteryCollectorCharacter.h"
#include "Kismet/GameplayStatics.h"
#include <Runtime/UMG/Public/Blueprint/UserWidget.h>
#include "SpawnVolume.h"

ABatteryCollectorGameMode::ABatteryCollectorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	decayRate = 0.01f;
}

void ABatteryCollectorGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundActors);

	for (auto Actor : FoundActors) {
		ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
		if (SpawnVolumeActor) {
			SpawnVolumeActors.AddUnique(SpawnVolumeActor);
		}
	}

	SetCurrentState(EBatteryPlayState::EPlaying);
	ABatteryCollectorCharacter* myCharacter = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (myCharacter)
	{
		powerToWin = myCharacter->GetInitialPower() * 1.25f;
	}

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

float ABatteryCollectorGameMode::GetPowerToWin() const
{
	return powerToWin;
}

EBatteryPlayState ABatteryCollectorGameMode::GetCurrentState() const
{
	return CurrentState;
}

void ABatteryCollectorGameMode::SetCurrentState(EBatteryPlayState NewState)
{
	CurrentState = NewState;

	HandleNewState(CurrentState);
}

void ABatteryCollectorGameMode::HandleNewState(EBatteryPlayState NewState) {
	switch (NewState)
	{
	case EBatteryPlayState::EPlaying:
		for (ASpawnVolume* Volume : SpawnVolumeActors) {
			Volume->SetSpawningActive(true);
		}
		break;
	case EBatteryPlayState::EGameOver: {
		for (ASpawnVolume* Volume : SpawnVolumeActors) {
			Volume->SetSpawningActive(false);
		}

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController) {
			PlayerController->SetCinematicMode(true, false, false, true, true);
		}

		ACharacter* Character = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (Character) {
			Character->GetMesh()->SetSimulatePhysics(true);
			Character->GetMovementComponent()->MovementState.bCanJump = false;
		}
		break;
	}
	case EBatteryPlayState::EWon: {
		for (ASpawnVolume* Volume : SpawnVolumeActors) {
			Volume->SetSpawningActive(false);
		}

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController) {
			PlayerController->SetCinematicMode(true, false, false, true, true);
		}
	}
		break;
	case EBatteryPlayState::EUnknown:
	default:

		break;
	}
}

void ABatteryCollectorGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ABatteryCollectorCharacter* myCharacter = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (myCharacter)
	{
		if ((myCharacter->GetCurrentPower()) > powerToWin) {
			SetCurrentState(EBatteryPlayState::EWon);
		}
		else if ((myCharacter->GetCurrentPower()) > 0) {
			myCharacter->UpdatePower(-DeltaTime * decayRate * (myCharacter->GetInitialPower()));
		}
		else {
			SetCurrentState(EBatteryPlayState::EGameOver);
		}
	}
}
