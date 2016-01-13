// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "BatteryCollectorGameMode.generated.h"

UENUM(BlueprintType)
enum class EBatteryPlayState {
	EPlaying,
	EGameOver,
	EWon,
	EUnknown
};

UCLASS(minimalapi)
class ABatteryCollectorGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABatteryCollectorGameMode();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWin() const;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category="Power")
	EBatteryPlayState GetCurrentState() const;

	void SetCurrentState(EBatteryPlayState NewState);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power")
	float decayRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float powerToWin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY()
	class UUserWidget* CurrentWidget;

private:
	EBatteryPlayState CurrentState;

	TArray<class ASpawnVolume*> SpawnVolumeActors;

	void HandleNewState(EBatteryPlayState NewState);
};



