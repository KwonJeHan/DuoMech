// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Game/DMGameState.h"
#include "DMStageManager.generated.h"

UCLASS()
class DUOMECH_API ADMStageManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ADMStageManager();

	UFUNCTION()
	void SetLaserActive(bool bActive);

	UFUNCTION()
	void RespawnPlayer(ADMCharacterPlayer* Player);
protected:

	UPROPERTY(EditInstanceOnly, Category = Reset)
	AActor* Player1Start;

	UPROPERTY(EditInstanceOnly, Category = Reset)
	AActor* Player2Start;

	UPROPERTY(EditInstanceOnly, Category = Reset)
	TArray<AActor*> StageObjectsToReset;

protected:
	UPROPERTY(EditInstanceOnly, Category = Laser)
	bool bIsLaserActive = false;
};
