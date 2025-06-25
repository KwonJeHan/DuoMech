// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DMStageResettableInterface.h"
#include "DMBlockerDoor.generated.h"

UCLASS()
class DUOMECH_API ADMBlockerDoor : public AActor, public IDMStageResettableInterface
{
	GENERATED_BODY()
	
public:	
	ADMBlockerDoor();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCOpen();

	virtual void ResetStage() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCResetBlockerDoorState();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;
};
