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

	// ����ȭ
	bReplicates = true;
	SetReplicateMovement(true);

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 600.0f;
	PawnSensing->SetPeripheralVisionAngle(40.0f);
	PawnSensing->SensingInterval = 0.1f;
	
	CurrentState = EExploderState::Patrol;

	// �þ� ��ä�� �޽� ����
	VisionConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisionConeMesh"));
	VisionConeMesh->SetupAttachment(RootComponent);
	VisionConeMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	VisionConeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisionConeMesh->SetCastShadow(false);
	VisionConeMesh->SetVisibility(true);

	// ĸ��
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_DMCAPSULE);

	// �⺻ �ȱ� �ӵ�
	GetCharacterMovement()->MaxWalkSpeed = 350.0f;

	// �޽� ������Ʈ ��� ��ġ, ȸ�� ����
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -16.0f), FRotator(0.0f, -90.0f, 0.0f));
	// �޽� ������Ʈ�� �ִϸ��̼� ��� ����
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	// �޽� ������Ʈ�� �浹 ������ ����
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// �޽ÿ� �ִϸ��̼� �������Ʈ ����
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

	// �þ� ��ä�� �޽� �� ��Ƽ����
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

    // ���� �����̰� Ÿ�� �÷��̾ ���� ��
    if (CurrentState == EExploderState::Chasing && TargetPlayer)
    {
		// AIController�� ����ȯ
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			// Ÿ�� �÷��̾�� �̵� ���
			AIController->MoveToActor(TargetPlayer);
		}
    }
	// ���� ������ ��
	else if (CurrentState == EExploderState::Patrol)
	{
		// ȸ�� ���̸�
		if (bIsRotating)
		{
			// ���� ȸ���� ����
			FRotator CurrentRotation = GetActorRotation();
			// ��ǥ ȸ�������� ����
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotationAfterPause, DeltaTime, RotationInterpSpeed);
			// ȸ�� ����
			SetActorRotation(NewRotation);

			// ��ǥ ȸ������ ���� ���� ���
			float YawDiff = FMath::Abs(FRotator::NormalizeAxis(NewRotation.Yaw - TargetRotationAfterPause.Yaw));

			// ���� ���̰� 1�� �����̸�
			if (YawDiff < 1.0f)
			{
				// ȸ�� ���� �� �̵� �÷��� ����
				bIsRotating = false;
				bIsCurrentlyMoving = false; // ���� �̵��� ���� �ʱ�ȭ
			}
			return; // ȸ�� ���̸� �Ʒ� ���� �������� ����
		}

		// ���� ������ ���
		FVector Destination = bGoingToTarget ? PatrolEndLocation : PatrolStartLocation;

		// AIController�� ����ȯ
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			// ���� ��ġ�� ������ �Ÿ� ���
			float Distance = FVector::Dist(GetActorLocation(), Destination);

			bool bReachedByDistance = Distance < PatrolAcceptanceRadius;
			bool bAIMovementStopped = (AIController->GetMoveStatus() == EPathFollowingStatus::Idle);

			// �������� �����ߴ��� Ȯ�� (�̵� ���� ����)
			if (bIsCurrentlyMoving && (bReachedByDistance || bAIMovementStopped))
			{
				// �̵� ����
				AIController->StopMovement();
				bIsCurrentlyMoving = false;

				// ���� �������� ���� ��ȯ
				bGoingToTarget = !bGoingToTarget;

				// ȸ���� ��ǥ ���� ���
				FVector NextDestination = bGoingToTarget ? PatrolEndLocation : PatrolStartLocation;
				TargetRotationAfterPause = (NextDestination - GetActorLocation()).Rotation();

				// ȸ�� ����
				bIsRotating = true;
			}
			else if(!bIsCurrentlyMoving && !bIsRotating)
			{
				// �������� �̵� ���
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
		// ��ƼŬ ���
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation()
		);

		// �޽ô� �Ⱥ��̵��� ó��(��Ȱ��ȭ)
		GetMesh()->SetHiddenInGame(true);

		// ������ �ݸ��� ����
		SetActorEnableCollision(false);

		SetActorTickEnabled(false);

		// AI ��Ȱ��ȭ
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->StopMovement();
		}

		// �þ� ���� ��Ȱ��ȭ
		if (PawnSensing)
		{
			PawnSensing->SetSensingUpdatesEnabled(false);
		}
	}

	// �÷��̾� üũ �� ���� ó��
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
	// �޽� Ȱ��ȭ
	GetMesh()->SetHiddenInGame(false);

	// ���� �ʱ�ȭ
	CurrentState = EExploderState::Patrol;
	TargetPlayer = nullptr;

	// ��ġ �ʱ�ȭ
	SetActorLocation(PatrolStartLocation);
	bGoingToTarget = true;

	// Ÿ�̸� ����
	GetWorld()->GetTimerManager().ClearTimer(ExplosionTimerHandle);

	// �浹/�ð�
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);

	// AI �簳
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToLocation(PatrolEndLocation);
	}

	VisionConeMesh->SetVisibility(true);

	// �þ� ���� ��Ȱ��ȭ
	if (PawnSensing)
	{
		PawnSensing->SetSensingUpdatesEnabled(true);
	}
}
