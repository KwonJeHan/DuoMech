// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/DMLaserZone.h"
#include "Components/StaticMeshComponent.h"
#include "Character/DMCharacterPlayer.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Physics/DMCollision.h"

// Sets default values
ADMLaserZone::ADMLaserZone()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	LaserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));
	LaserMesh->SetupAttachment(RootComponent);

	LaserMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LaserMesh->SetGenerateOverlapEvents(true);
	LaserMesh->SetCollisionProfileName("OverlapAll");
	LaserMesh->SetVisibility(true);
	LaserMesh->SetCastShadow(false);
	LaserMesh->SetIsReplicated(true);

	LaserEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserEffect"));
	LaserEffect->SetupAttachment(RootComponent);
	LaserEffect->SetRelativeScale3D(FVector(1.0f));
	LaserEffect->SetRelativeRotation(FRotator::ZeroRotator);
	LaserEffect->SetRelativeLocation(FVector::ZeroVector);
	LaserEffect->SetAutoActivate(false);
	LaserEffect->SetIsReplicated(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> LaserMeshRef(
		TEXT("/Game/DuoMech/Props/LaserCube.LaserCube"));
	if (LaserMeshRef.Object)
	{
		LaserMesh->SetStaticMesh(LaserMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> LaserMatRef(
		TEXT("/Game/DuoMech/Material/M_LaserMaterial.M_LaserMaterial"));
	if (LaserMatRef.Object)
	{
		LaserMesh->SetMaterial(0, LaserMatRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LaserEffectRef(
		TEXT("/Game/BeamsPack/VFX/Beams/NS_BeamOnly_05.NS_BeamOnly_05"));
	if (LaserEffectRef.Object)
	{
		LaserEffect->SetAsset(LaserEffectRef.Object);
	}

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ADMLaserZone::BeginPlay()
{
	Super::BeginPlay();

	// 메시 크기 조정
	if (LaserMesh && LaserEffect)
	{
		// 길고 얇은 레이저
		LaserMesh->SetRelativeScale3D(FVector(0.5f, 20.0f, 0.1f));
		LaserEffect->SetRelativeLocation(FVector(0.0f, -1000.0f, 0.0f));
		LaserEffect->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
		LaserEffect->SetRelativeScale3D(FVector(6.0f, 5.0f, 3.5f));

		//파티클 재생
		LaserEffect->ActivateSystem();
	}


	// 1초 뒤 자동 파괴
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			FTimerHandle LaserTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				LaserTimerHandle,
				this,
				&ADMLaserZone::DestroySelf,
				1.0f,
				false);
		});
}

void ADMLaserZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		TArray<AActor*> OverlappingActors;
		LaserMesh->GetOverlappingActors(OverlappingActors, ADMCharacterPlayer::StaticClass());

		for (AActor* Actor : OverlappingActors)
		{
			if (ADMCharacterPlayer* Player = Cast<ADMCharacterPlayer>(Actor))
			{
				Player->SetDead();
			}
		}
	}
}

void ADMLaserZone::DestroySelf()
{
	if (LaserEffect)
	{
		LaserEffect->Deactivate();
	}
	Destroy();
}