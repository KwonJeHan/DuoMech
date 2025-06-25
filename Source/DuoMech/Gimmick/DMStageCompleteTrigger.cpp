// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/DMStageCompleteTrigger.h"
#include "Character/DMCharacterPlayer.h"
#include "DMStageManager.h"

void ADMStageCompleteTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ADMCharacterPlayer* Player = Cast<ADMCharacterPlayer>(OtherActor))
	{
		// ���� �ʵ��� ����
		Player->SetCanDie(false);

		if (ADMStageManager* Stage = FindStageManager())
		{
			Stage->SetLaserActive(false);
		}
	}
}
