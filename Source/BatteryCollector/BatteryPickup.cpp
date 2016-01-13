// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "BatteryPickup.h"

ABatteryPickup::ABatteryPickup() {
	GetMesh()->SetSimulatePhysics(true);

	BatteryPower = 150;
}

void ABatteryPickup::WasCollected_Implementation(){
	Super::WasCollected_Implementation();

	Destroy();
}

float ABatteryPickup::GetPower()
{
	return BatteryPower;
}