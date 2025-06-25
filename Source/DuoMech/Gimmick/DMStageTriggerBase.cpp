// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/DMStageTriggerBase.h"
#include "Components/BoxComponent.h"
#include "EngineUtils.h"

// Sets default values
ADMStageTriggerBase::ADMStageTriggerBase()
{
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
}

void ADMStageTriggerBase::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ADMStageTriggerBase::OnOverlapBegin);
	
}

void ADMStageTriggerBase::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
}

ADMStageManager* ADMStageTriggerBase::FindStageManager()
{
	for (TActorIterator<ADMStageManager> It(GetWorld()); It; ++It)
	{
		return *It;
	}
	return nullptr;
}
