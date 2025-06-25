// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/DMBlockerDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ADMBlockerDoor::ADMBlockerDoor()
{
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = DoorMesh;

	/*DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);*/
	DoorMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// 메시 애셋
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorMeshRef(
		TEXT("/Game/Kobo_Dungeon/Meshes/SM-Gate-01-02.SM-Gate-01-02"));
	if (DoorMeshRef.Object)
	{
		DoorMesh->SetStaticMesh(DoorMeshRef.Object);
	}

	bReplicates = true;
}

void ADMBlockerDoor::MulticastRPCOpen_Implementation()
{
	if (DoorMesh)
	{
		DoorMesh->SetHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

void ADMBlockerDoor::ResetStage()
{
	MulticastRPCResetBlockerDoorState();
}

void ADMBlockerDoor::MulticastRPCResetBlockerDoorState_Implementation()
{
	DoorMesh->SetHiddenInGame(false);
	SetActorEnableCollision(true);
}

