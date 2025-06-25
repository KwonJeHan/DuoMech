// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMStageManager.h"
#include "DMStageTriggerBase.generated.h"


UCLASS()
class DUOMECH_API ADMStageTriggerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ADMStageTriggerBase();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> TriggerVolume;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult);

	ADMStageManager* FindStageManager();
};
