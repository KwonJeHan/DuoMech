// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/DMStageManager.h"
#include "Interface/DMStageResettableInterface.h"
#include "Character/DMCharacterPlayer.h"
#include "Player/DMPlayerController.h"
#include "GameFramework/PlayerStart.h"


ADMStageManager::ADMStageManager()
{
}

void ADMStageManager::SetLaserActive(bool bActive)
{
	bIsLaserActive = bActive;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ADMCharacterPlayer* Player = Cast<ADMCharacterPlayer>((*It)->GetPawn()))
		{
			Player->SetLaserTickEnabled(bActive);
		}
	}
}

void ADMStageManager::RespawnPlayer(ADMCharacterPlayer* Player)
{
	for (AActor* Actor : StageObjectsToReset)
	{
		if (IDMStageResettableInterface* Resettable = Cast<IDMStageResettableInterface>(Actor))
		{
			Resettable->ResetStage();
		}
	}

	ADMPlayerController* DMPlayerController = Cast<ADMPlayerController>(Player->GetController());
	EPlayerRole PlayerRole = DMPlayerController->GetPlayerRole();
	AActor* StartPoint = nullptr;

	// 역할에 따라 스폰 위치 결정
	switch (PlayerRole)
	{
	case EPlayerRole::Player1:
		StartPoint = Player1Start;
		if (Player)
		{
			Player->SetActorLocation(StartPoint->GetActorLocation());
			Player->SetActorRotation(StartPoint->GetActorRotation());
			Player->MulticastRPCResetState();
		}
		break;
	case EPlayerRole::Player2:
		StartPoint = Player2Start;
		if (Player)
		{
			Player->SetActorLocation(StartPoint->GetActorLocation());
			Player->SetActorRotation(StartPoint->GetActorRotation());
			Player->MulticastRPCResetState();
		}
		break;
	}

	if (!StartPoint)
	{
		return;
	}
}
