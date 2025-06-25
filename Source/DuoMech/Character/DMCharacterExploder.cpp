// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DMCharacterExploder.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "DMCharacterPlayer.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Physics/DMCollision.h"
#include "Net/UnrealNetwork.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"

// Sets default values
ADMCharacterExploder::ADMCharacterExploder()
{
	PrimaryActorTick.bCanEverTick = true;

	// 동기화
	bReplicates = true;
	SetReplicateMovement(true);

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 600.0f;
	PawnSensing->SetPeripheralVisionAngle(40.0f);
	PawnSensing->SensingInterval = 0.1f;
	
	CurrentState = EExploderState::Patrol;

	// 시야 부채꼴 메시 생성
	VisionConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisionConeMesh"));
	VisionConeMesh->SetupAttachment(RootComponent);
	VisionConeMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	VisionConeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisionConeMesh->SetCastShadow(false);
	VisionConeMesh->SetVisibility(true);

	// 캡슐
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_DMCAPSULE);

	// 기본 걷기 속도
	GetCharacterMovement()->MaxWalkSpeed = 350.0f;

	// 메시 컴포넌트 상대 위치, 회전 설정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -16.0f), FRotator(0.0f, -90.0f, 0.0f));
	// 메시 컴포넌트의 애니메이션 모드 설정
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	// 메시 컴포넌트의 충돌 프로필 설정
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// 메시와 애니메이션 블루프린트 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> AICharacterMeshRef(
		TEXT("/Game/MilitaryDrone/Meshes/drone_model.drone_model"));
	if (AICharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(AICharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AIAnimInstanceClassRef(
		TEXT("/Game/DuoMech/Animation/ABP_DMExploder.ABP_DMExploder_C"));
	if (AIAnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AIAnimInstanceClassRef.Class);
	}
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExplosionEffectRef(
		TEXT("/Game/sA_ExplosionPack_2/Fx/NS_Explosion_2.NS_Explosion_2"));
	if (ExplosionEffectRef.Object)
	{
		ExplosionEffect = ExplosionEffectRef.Object;
	}

	// 시야 부채꼴 메시 및 머티리얼
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshRef(
		TEXT("/Game/DuoMech/Props/VisionCone.VisionCone"));
	if (ConeMeshRef.Object)
	{
		VisionConeMesh->SetStaticMesh(ConeMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ConeMatRef(
		TEXT("/Game/DuoMech/Material/M_SightMaterial.M_SightMaterial"));
	if (ConeMatRef.Object)
	{
		VisionConeMesh->SetMaterial(0, ConeMatRef.Object);
	}

}

void ADMCharacterExploder::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADMCharacterExploder, CurrentState);
}

void ADMCharacterExploder::BeginPlay()
{
	Super::BeginPlay();
	
	if (VisionConeMesh && PawnSensing)
	{
		float Radius = PawnSensing->SightRadius;
		VisionConeMesh->SetWorldScale3D(FVector(1.0f, Radius / 70.0f, Radius / 130.0f));
		PawnSensing->OnSeePawn.AddDynamic(this, &ADMCharacterExploder::OnSeePawn);
	}

	PatrolStartLocation = GetActorLocation();
	PatrolEndLocation = PatrolStartLocation + GetActorForwardVector() * PatrolDistance;
}

void ADMCharacterExploder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 추적 상태이고 타겟 플레이어가 있을 때
    if (CurrentState == EExploderState::Chasing && TargetPlayer)
    {
		// AIController로 형변환
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			// 타겟 플레이어에게 이동 명령
			AIController->MoveToActor(TargetPlayer);
		}
    }
	// 순찰 상태일 때
	else if (CurrentState == EExploderState::Patrol)
	{
		// 회전 중이면
		if (bIsRotating)
		{
			// 현재 회전값 저장
			FRotator CurrentRotation = GetActorRotation();
			// 목표 회전값으로 보간
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotationAfterPause, DeltaTime, RotationInterpSpeed);
			// 회전 적용
			SetActorRotation(NewRotation);

			// 목표 회전과의 각도 차이 계산
			float YawDiff = FMath::Abs(FRotator::NormalizeAxis(NewRotation.Yaw - TargetRotationAfterPause.Yaw));

			// 각도 차이가 1도 이하이면
			if (YawDiff < 1.0f)
			{
				// 회전 종료 및 이동 플래그 설정
				bIsRotating = false;
				bIsCurrentlyMoving = false; // 다음 이동을 위해 초기화
			}
			return; // 회전 중이면 아래 로직 실행하지 않음
		}

		// 현재 목적지 계산
		FVector Destination = bGoingToTarget ? PatrolEndLocation : PatrolStartLocation;

		// AIController로 형변환
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			// 현재 위치와 목적지 거리 계산
			float Distance = FVector::Dist(GetActorLocation(), Destination);

			bool bReachedByDistance = Distance < PatrolAcceptanceRadius;
			bool bAIMovementStopped = (AIController->GetMoveStatus() == EPathFollowingStatus::Idle);

			// 목적지에 도달했는지 확인 (이동 중일 때만)
			if (bIsCurrentlyMoving && (bReachedByDistance || bAIMovementStopped))
			{
				// 이동 중지
				AIController->StopMovement();
				bIsCurrentlyMoving = false;

				// 다음 목적지로 방향 전환
				bGoingToTarget = !bGoingToTarget;

				// 회전할 목표 각도 계산
				FVector NextDestination = bGoingToTarget ? PatrolEndLocation : PatrolStartLocation;
				TargetRotationAfterPause = (NextDestination - GetActorLocation()).Rotation();

				// 회전 시작
				bIsRotating = true;
			}
			else if(!bIsCurrentlyMoving && !bIsRotating)
			{
				// 목적지로 이동 명령
				AIController->MoveToLocation(Destination);
				bIsCurrentlyMoving = true;
			}
		}
    }
}

void ADMCharacterExploder::OnSeePawn(APawn* Pawn)
{
	if (CurrentState != EExploderState::Patrol)
	{
		return;
	}
	
	TargetPlayer = Pawn;
	SetState(EExploderState::Detected);
	MulticastRPCHideVisionCone();

	UE_LOG(LogTemp, Display, TEXT("Detected!!!"));

	StartExplosionCountdown();
}

void ADMCharacterExploder::MulticastRPCHideVisionCone_Implementation()
{
	VisionConeMesh->SetVisibility(false);
}

void ADMCharacterExploder::StartExplosionCountdown()
{
	SetState(EExploderState::Chasing);
	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle, 
		this,
		&ADMCharacterExploder::Explode, 
		ExplosionDelay, 
		false);
}

void ADMCharacterExploder::Explode()
{
	if (CurrentState == EExploderState::Dead)
	{
		return;
	}

	SetState(EExploderState::Exploding);

	MulticastRPCExplodeEffect();

	SetState(EExploderState::Dead);
}

void ADMCharacterExploder::MulticastRPCExplodeEffect_Implementation()
{
	if (ExplosionEffect)
	{
		// 파티클 재생
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation()
		);

		// 메시는 안보이도록 처리(비활성화)
		GetMesh()->SetHiddenInGame(true);

		// 액터의 콜리전 끄기
		SetActorEnableCollision(false);

		SetActorTickEnabled(false);

		// AI 비활성화
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->StopMovement();
		}

		// 시야 감지 비활성화
		if (PawnSensing)
		{
			PawnSensing->SetSensingUpdatesEnabled(false);
		}
	}

	// 플레이어 체크 및 피해 처리
	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor != this && FVector::Dist(GetActorLocation(), Actor->GetActorLocation()) <= ExplosionRadius)
		{
			if (ADMCharacterPlayer* DMPlayer = Cast<ADMCharacterPlayer>(Actor))
			{
				DMPlayer->SetDead();
			}
		}
	}
}

void ADMCharacterExploder::SetState(EExploderState NewState)
{
	CurrentState = NewState;
}

void ADMCharacterExploder::ResetStage()
{
	MulticastRPCResetExploderState();
}

void ADMCharacterExploder::MulticastRPCResetExploderState_Implementation()
{
	// 메시 활성화
	GetMesh()->SetHiddenInGame(false);

	// 상태 초기화
	CurrentState = EExploderState::Patrol;
	TargetPlayer = nullptr;

	// 위치 초기화
	SetActorLocation(PatrolStartLocation);
	bGoingToTarget = true;

	// 타이머 제거
	GetWorld()->GetTimerManager().ClearTimer(ExplosionTimerHandle);

	// 충돌/시각
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);

	// AI 재개
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToLocation(PatrolEndLocation);
	}

	VisionConeMesh->SetVisibility(true);

	// 시야 감지 재활성화
	if (PawnSensing)
	{
		PawnSensing->SetSensingUpdatesEnabled(true);
	}
}
