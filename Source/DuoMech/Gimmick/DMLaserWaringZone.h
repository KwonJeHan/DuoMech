// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMLaserWaringZone.generated.h"

UCLASS()
class DUOMECH_API ADMLaserWaringZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ADMLaserWaringZone();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> WarningMesh;

	UFUNCTION()
	void DestroySelf();
};
