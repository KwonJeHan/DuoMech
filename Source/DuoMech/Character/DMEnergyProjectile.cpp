// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DMEnergyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Physics/DMCollision.h"

// Sets default values
ADMEnergyProjectile::ADMEnergyProjectile()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(15.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &ADMEnergyProjectile::OnHit);
	RootComponent = CollisionComponent;

	// 메시 컴포넌트 어떤거 사용할 지 지정 필요!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(
		TEXT("/Game/Pirate_Props/StaticMesh/SM_Pirate_Props_ue5__pirate_boomb.SM_Pirate_Props_ue5__pirate_boomb"));
	if (MeshRef.Object)
	{
		MeshComponent->SetStaticMesh(MeshRef.Object);
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1500.0f;
	ProjectileMovement->MaxSpeed = 1500.0f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;

	// 추가
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->SetIsReplicated(true);

	InitialLifeSpan = 3.0f;

	// 발사체 동기화
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ADMEnergyProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ADMEnergyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Display, TEXT("Hit!!!!"));
	// TODO : 데미지 및 이펙트 처리
	Destroy();
}

