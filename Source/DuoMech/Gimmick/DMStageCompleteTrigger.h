// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/DMStageTriggerBase.h"
#include "DMStageCompleteTrigger.generated.h"

/**
 * 
 */
UCLASS()
class DUOMECH_API ADMStageCompleteTrigger : public ADMStageTriggerBase
{
	GENERATED_BODY()
	
protected:
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
};
