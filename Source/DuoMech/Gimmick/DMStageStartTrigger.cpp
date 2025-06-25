// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/DMStageStartTrigger.h"
#include "Character/DMCharacterPlayer.h"
#include "DMStageManager.h"

void ADMStageStartTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ADMCharacterPlayer* Player = Cast<ADMCharacterPlayer>(OtherActor))
	{
		// Á×µµ·Ï ¼³Á¤
		Player->SetCanDie(true);

		if (ADMStageManager* Stage = FindStageManager())
		{
			Stage->SetLaserActive(true);
		}
	}
}
