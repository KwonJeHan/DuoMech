// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/DMLaserWaringZone.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ADMLaserWaringZone::ADMLaserWaringZone()
{
	WarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WarningMesh"));
	RootComponent = WarningMesh;

	WarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WarningMesh->SetVisibility(true);
	// 레이저와 같은 크기
	WarningMesh->SetRelativeScale3D(FVector(0.5f, 20.0f, 0.5f));
	WarningMesh->SetCastShadow(false);
	WarningMesh->SetIsReplicated(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WarningMeshRef(
		TEXT("/Game/DuoMech/Props/LaserCube.LaserCube"));
	if (WarningMeshRef.Object)
	{
		WarningMesh->SetStaticMesh(WarningMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> WarningMatRef(
		TEXT("/Game/DuoMech/Material/M_WarningMaterial.M_WarningMaterial"));
	if (WarningMatRef.Object)
	{
		WarningMesh->SetMaterial(0, WarningMatRef.Object);
	}

	bReplicates = true;
	SetReplicateMovement(true);
}

void ADMLaserWaringZone::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			FTimerHandle WarningTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				WarningTimerHandle,
				this,
				&ADMLaserWaringZone::DestroySelf,
				2.0f,
				false);
		});
}

void ADMLaserWaringZone::DestroySelf()
{
	Destroy();
}


