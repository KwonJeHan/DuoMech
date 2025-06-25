// Fill out your copyright notice in the Description page of Project Settings.


#include "DMCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DMCharacterControlData.h"
#include "DMShootActionData.h"
#include "Player/DMPlayerController.h"
#include "Game/DMGameState.h"
#include "Game/DMGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Interface/DMStageResettableInterface.h"
#include "EngineUtils.h"
#include "Gimmick/DMLaserZone.h"
#include "Gimmick/DMLaserWaringZone.h"
#include "GameFramework/PlayerStart.h"
#include "HUD/DMPlayer1HUDWidget.h"
#include "HUD/DMPlayer2HUDWidget.h"

ADMCharacterPlayer::ADMCharacterPlayer()
{
	Tags.Add(FName("Player"));

	// 카메라
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;	// 카메라 붐의 길이
	CameraBoom->bUsePawnControlRotation = true;	// 카메라 붐이 플레이어의 회전을 따르도록 설정

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;	// 카메라가 플레이어의 회전을 따르지 않도록 설정

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Player1MatRef(
		TEXT("/Game/MikeDoublePack/Robot02/Materials/MI_Robot02_DefaultColor.MI_Robot02_DefaultColor"));
	if (Player1MatRef.Object)
	{
		Player1Material = Player1MatRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Player2MatRef(
		TEXT("/Game/MikeDoublePack/Robot02/Materials/MI_Robot02_Green.MI_Robot02_Green"));
	if (Player2MatRef.Object)
	{
		Player2Material = Player2MatRef.Object;
	}

	// Forward 입력 매핑 컨텍스트 설정
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ForwardInputMappingContextRef(
		TEXT("/Game/DuoMech/Input/IMC_Forward.IMC_Forward"));
	if (ForwardInputMappingContextRef.Object)
	{
		DefaultMappingContext = ForwardInputMappingContextRef.Object;
	}

	// Right 입력 매핑 컨텍스트 설정
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> RightInputMappingContextRef(
		TEXT("/Game/DuoMech/Input/IMC_Right.IMC_Right"));
	if (RightInputMappingContextRef.Object)
	{
		DefaultMappingContext = RightInputMappingContextRef.Object;
	}

	// Forward 이동 액션 설정
	static ConstructorHelpers::FObjectFinder<UInputAction> ForwardMoveActionRef(
		TEXT("/Game/DuoMech/Input/Actions/IA_ForwardMove.IA_ForwardMove"));
	if (ForwardMoveActionRef.Object)
	{
		ForwardMoveAction = ForwardMoveActionRef.Object;
	}

	// Right 이동 액션 설정
	static ConstructorHelpers::FObjectFinder<UInputAction> RightMoveActionRef(
		TEXT("/Game/DuoMech/Input/Actions/IA_RightMove.IA_RightMove"));
	if (RightMoveActionRef.Object)
	{
		RightMoveAction = RightMoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ShootActionMontageRef(
		TEXT("/Game/DuoMech/Animation/AM_Shoot.AM_Shoot"));
	if (ShootActionMontageRef.Object)
	{
		ShootMontage = ShootActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDMShootActionData> ShootActionDataRef(
		TEXT("/Game/DuoMech/CharacterAction/DMS_ShootAction.DMS_ShootAction"));
	if (ShootActionDataRef.Object)
	{
		ShootActionData = ShootActionDataRef.Object;
	}

	// 발사 액션 설정
	static ConstructorHelpers::FObjectFinder<UInputAction> ShootActionRef(
		TEXT("/Game/DuoMech/Input/Actions/IA_Shoot.IA_Shoot"));
	if (ShootActionRef.Object)
	{
		ShootAction = ShootActionRef.Object;
	}

	// Projectile 클래스 어떤거 사용할 지 지정 필요!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	static ConstructorHelpers::FClassFinder<ADMEnergyProjectile> ProjectileClassRef(
		TEXT("/Game/DuoMech/Blueprint/BP_EnergyProjectile.BP_EnergyProjectile_C"));
	if (ProjectileClassRef.Class)
	{
		ProjectileClass = ProjectileClassRef.Class;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(
		TEXT("/Game/DuoMech/Animation/AM_Dead.AM_Dead"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::Forward;
	bReplicates = true;
}

void ADMCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMCharacterPlayer, bCanDie)
}

void ADMCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 필요한 초기화 작업
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}

	SetCharacterControl(CurrentCharacterControlType);
}

void ADMCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ADMCharacterPlayer::OnRep_Owner()
{
	Super::OnRep_Owner();
}

void ADMCharacterPlayer::PostNetInit()
{
	Super::PostNetInit();
}

void ADMCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input Component로 캐스팅
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// Enhanced Input Subsystem을 가져와서 기본 입력 매핑 컨텍스트를 추가
	EnhancedInputComponent->BindAction(ForwardMoveAction, ETriggerEvent::Triggered, this, &ADMCharacterPlayer::MoveForward);
	EnhancedInputComponent->BindAction(ForwardMoveAction, ETriggerEvent::Completed, this, &ADMCharacterPlayer::MoveForward);
	EnhancedInputComponent->BindAction(RightMoveAction, ETriggerEvent::Triggered, this, &ADMCharacterPlayer::MoveRight);
	EnhancedInputComponent->BindAction(RightMoveAction, ETriggerEvent::Completed, this, &ADMCharacterPlayer::MoveRight);
	EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ADMCharacterPlayer::Shoot);

}

void ADMCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	UDMCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void ADMCharacterPlayer::SetCharacterControlData(const UDMCharacterControlData* InCharacterControlData)
{
	Super::SetCharacterControlData(InCharacterControlData);

	// 카메라 스프링 암 설정	
	CameraBoom->TargetArmLength = InCharacterControlData->TargetArmLength;		// 카메라 붐의 길이 설정
	CameraBoom->SetRelativeRotation(InCharacterControlData->RelativeRotation);	// 카메라 붐의 상대 회전 설정
	CameraBoom->bUsePawnControlRotation = InCharacterControlData->bUsePawnControlRotation;	// 카메라 붐이 플레이어의 회전을 따르도록 설정
	CameraBoom->bInheritPitch = InCharacterControlData->bInheritPitch;			// 카메라 붐이 피치를 상속받도록 설정
	CameraBoom->bInheritYaw = InCharacterControlData->bInheritYaw;				// 카메라 붐이 요를 상속받도록 설정
	CameraBoom->bInheritRoll = InCharacterControlData->bInheritRoll;				// 카메라 붐이 롤을 상속받도록 설정
	CameraBoom->bDoCollisionTest = InCharacterControlData->bDoCollisionTest;		// 카메라 붐이 충돌 테스트를 수행하도록 설정
}

void ADMCharacterPlayer::SetDead()
{
	if (!bCanDie)
	{
		return;
	}

	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

		if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
		{
			if (ADMGameMode* DMGameMode = Cast<ADMGameMode>(GameMode))
			{
				DMGameMode->KillAllPlayersAndStartRespawn();
			}
		}
		MulticastRPCOnDeath();

		// 5초 뒤 리스폰
		GetWorld()->GetTimerManager().SetTimer(
			RespawnTimerHandle,
			this,
			&ADMCharacterPlayer::RequestRespawn,
			5.0f, false);
	}
}

void ADMCharacterPlayer::SetCanDie(bool bNewCanDie)
{
	if (HasAuthority())
	{
		bCanDie = bNewCanDie;
		OnRep_CanDie();
	}
}

void ADMCharacterPlayer::OnRep_CanDie()
{
}

void ADMCharacterPlayer::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.0f);

		const float PlayRate = 1.0f;
		AnimInstance->Montage_Play(DeadMontage, PlayRate);
	}
}

void ADMCharacterPlayer::MulticastRPCOnDeath_Implementation()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);

	if (ADMStageManager* Stage = FindStageManager())
	{
		Stage->SetLaserActive(false);
	}
}

void ADMCharacterPlayer::RequestRespawn()
{
	if (ADMStageManager* Stage = FindStageManager())
	{
		Stage->RespawnPlayer(this);
	}
}

ADMStageManager* ADMCharacterPlayer::FindStageManager()
{
	for (TActorIterator<ADMStageManager> It(GetWorld()); It; ++It)
	{
		return *It;
	}
	return nullptr;
}

void ADMCharacterPlayer::MulticastRPCResetState_Implementation()
{
	bIsDead = false;
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->StopAllMontages(0.f);
	}
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	SetActorEnableCollision(true);
}

void ADMCharacterPlayer::MoveForward(const FInputActionValue& Value)
{
	// 입력 값에서 FVector2D를 가져옴
	CachedMovementVector = Value.Get<FVector2D>();

	// 공격 중일 때 못 움직이도록
	if (const ADMGameState* DMGameState = GetWorld()->GetGameState<ADMGameState>())
	{
		if (DMGameState && DMGameState->GetIsAttacking())
		{
			return;
		}
	}

	UpdateHUDDirectionState(CachedMovementVector);

	// 로컬에서 움직임 처리
	ProcessMovement(CachedMovementVector, true);	// true는 ForwardMovement를 의미

	// 네트워크를 통해 다른 플레이어에게 움직임 전송
	if (ADMPlayerController* DMPlayerController = Cast<ADMPlayerController>(GetController()))
	{
		// 움직임 데이터 생성
		FMovementData MovementData(CachedMovementVector, DMPlayerController->GetPlayerRole(), true);
		// 서버로 움직임 데이터 전송
		DMPlayerController->ServerRPCReceiveMovementInput(MovementData);
	}
}

void ADMCharacterPlayer::MoveRight(const FInputActionValue& Value)
{
	// 입력 값에서 FVector2D를 가져옴
	CachedMovementVector = Value.Get<FVector2D>();

	// 공격 중일 때 못 움직이도록
	if (const ADMGameState* DMGameState = GetWorld()->GetGameState<ADMGameState>())
	{
		if (DMGameState && DMGameState->GetIsAttacking())
		{
			return;
		}
	}

	UpdateHUDDirectionState(CachedMovementVector);

	// 로컬에서 움직임 처리
	ProcessMovement(CachedMovementVector, false);	// false는 RightMovement를 의미

	// 네트워크를 통해 다른 플레이어에게 움직임 전송
	if (ADMPlayerController* DMPlayerController = Cast<ADMPlayerController>(GetController()))
	{
		// 움직임 데이터 생성
		FMovementData MovementData(CachedMovementVector, DMPlayerController->GetPlayerRole(), false);
		// 서버로 움직임 데이터 전송
		DMPlayerController->ServerRPCReceiveMovementInput(MovementData);
	}
}

void ADMCharacterPlayer::MoveStopped(const FInputActionValue& Value)
{
	CachedMovementVector = FVector2D::ZeroVector;

	// HUD 갱신
	UpdateHUDDirectionState(CachedMovementVector);
}

void ADMCharacterPlayer::ProcessMovement(const FVector2D& MovementVector, bool bIsForwardMovement)
{
	// 기본 이동 벡터 크기 설정
	float MovementVectorSize = 1.0f;

	// 이동 벡터를 정규화하기 위한 변수
	FVector2D NormalizedMovement = MovementVector;

	// 이동 벡터의 크기 제곱을 계산
	float MovementVectorSizeSquared = MovementVector.SquaredLength();

	if (MovementVectorSizeSquared > 1.0f)	// 이동 벡터의 크기 제곱이 1보다 크면
	{
		NormalizedMovement.Normalize();		// 크기를 1로 정규화
		MovementVectorSizeSquared = 1.0f;	// 정규화된 벡터의 크기 제곱을 1로 설정
	}
	else
	{
		// 이동 벡터의 크기 제곱이 1보다 작거나 같으면, 크기를 그대로 사용
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	// 모든 플레이어가 모든 움직임에 대해 회전 적용
	// 움직임이 있고 컨트롤러가 유효할 때만 회전 설정
	if (MovementVectorSize > 0.0f && Controller)
	{
		// 이동 방향 벡터 생성
		FVector MoveDirection = FVector(NormalizedMovement.X, NormalizedMovement.Y, 0.0f);

		FRotator TargetRotation = FRotationMatrix::MakeFromX(MoveDirection).Rotator();
		Controller->SetControlRotation(TargetRotation);

		// 이동 입력 추가
		AddMovementInput(MoveDirection, MovementVectorSize);
	}
}

void ADMCharacterPlayer::ApplyNetworkedMovement(const FMovementData& MovementData)
{
	// 다른 플레이어로부터 받은 움직임을 적용
	// 네트워크를 통해 받은 움직임이므로 로컬에서만 처리하고 다시 전송하지 않음
	ProcessMovement(MovementData.MovementInput, MovementData.bIsForwardMovement);
}


// Shoot Section
void ADMCharacterPlayer::Shoot()
{
	if (!bHasBuster)
	{
		return;
	}
	
	if (!HasAuthority())
	{
		// 서버에 발사 요청
		ServerRPCShoot();
		return;
	}

	// 공격 중일 때 다시 공격 불가능
	if (ADMGameState* DMGameState = GetWorld()->GetGameState<ADMGameState>())
	{
		if (DMGameState->GetIsAttacking())
		{
			return;
		}
	
		SetAttackState(true);
		SpawnProjectile();

		// 모든 클라이언트에 애니메이션 알림
		MulticastRPCPlayShootMontage();
	}
	
}

void ADMCharacterPlayer::ServerRPCShoot_Implementation()
{
	Shoot();
}

void ADMCharacterPlayer::MulticastRPCPlayShootMontage_Implementation()
{
	if (ShootMontage && GetMesh())
	{
		PlayAnimMontage(ShootMontage);

		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &ADMCharacterPlayer::OnShootMontageEnded);

			AnimInstance->Montage_SetEndDelegate(EndDelegate, ShootMontage);
		}
	}
}

// Projectile 생성
void ADMCharacterPlayer::SpawnProjectile()
{
	if (!ProjectileClass)
	{
		return;
	}

	FVector MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator MuzzleRotation = GetActorRotation();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();

	GetWorld()->SpawnActor<ADMEnergyProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);
}

void ADMCharacterPlayer::OnShootMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	SetAttackState(false);
}

void ADMCharacterPlayer::SetAttackState(bool bAttacking)
{
	if (ADMGameState* DMGameState = GetWorld()->GetGameState<ADMGameState>())
	{
		DMGameState->SetIsAttacking(bAttacking);
	}
}

void ADMCharacterPlayer::SetLaserTickEnabled(bool bActive)
{
	// 주기적으로 레이저 생성
	if (HasAuthority())
	{
		if (bActive)
		{
			// 3초 뒤 레이저 생성 시작
			GetWorld()->GetTimerManager().SetTimer(
				Player1LaserTimerHandle, this,
				&ADMCharacterPlayer::SpawnLaserForPlayer1,
				8.0f, true, 3.0f);

			// 6초 뒤 레이저 생성 시작
			GetWorld()->GetTimerManager().SetTimer(
				Player2LaserTimerHandle, this,
				&ADMCharacterPlayer::SpawnLaserForPlayer2,
				8.0f, true, 6.0f);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(Player1LaserTimerHandle);
			GetWorld()->GetTimerManager().ClearTimer(Player2LaserTimerHandle);
		}
	}
}

void ADMCharacterPlayer::UpdateHUDDirectionState(const FVector2D& MovementVector)
{
	if (ADMPlayerController* PC = Cast<ADMPlayerController>(GetController()))
	{
		if (UDMInGameHUDWidget* HUD = Cast<UDMInGameHUDWidget>(PC->GetHUDWidget()))
		{
			HUD->SetKeyVisualState(EInputDirection::Up, MovementVector.X > 0.1f);
			HUD->SetKeyVisualState(EInputDirection::Down, MovementVector.X < -0.1f);
			HUD->SetKeyVisualState(EInputDirection::Right, MovementVector.Y > 0.1f);
			HUD->SetKeyVisualState(EInputDirection::Left, MovementVector.Y < -0.1f);
		}
	}
}

void ADMCharacterPlayer::SpawnLaserForPlayer1()
{
	if (HasAuthority())
	{
		if (ADMPlayerController* DMPlayerController = Cast<ADMPlayerController>(GetController()))
		{
			if (DMPlayerController->GetPlayerRole() == EPlayerRole::Player1)
			{
				FVector SpawnLocation = GetActorLocation();
				// 좌우 회피 (세로 레이저)
				FRotator Rotation = FRotator(0.0f, 90.0f, 0.0f);

				FVector LocationCopy = SpawnLocation;
				FRotator RotationCopy = Rotation;

				// 레이저 예고 표시
				GetWorld()->SpawnActor<ADMLaserWaringZone>(ADMLaserWaringZone::StaticClass(), SpawnLocation, Rotation);

				// 2초 뒤 레이저 본체 생성
				FTimerHandle Spawn1TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					Spawn1TimerHandle,
					FTimerDelegate::CreateLambda([LocationCopy, RotationCopy, this]()
						{
							// 레이저 생성
							GetWorld()->SpawnActor<ADMLaserZone>(ADMLaserZone::StaticClass(), LocationCopy, RotationCopy);
						}), 2.0f, false);
			}
		}
	}
}

void ADMCharacterPlayer::SpawnLaserForPlayer2()
{
	if (HasAuthority())
	{
		if (ADMPlayerController* DMPlayerController = Cast<ADMPlayerController>(GetController()))
		{
			if (DMPlayerController->GetPlayerRole() == EPlayerRole::Player2)
			{
				FVector SpawnLocation = GetActorLocation();
				// 앞뒤 회피 (가로 레이저)
				FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f);

				FVector LocationCopy = SpawnLocation;
				FRotator RotationCopy = Rotation;

				// 레이저 예고 표시
				GetWorld()->SpawnActor<ADMLaserWaringZone>(ADMLaserWaringZone::StaticClass(), SpawnLocation, Rotation);

				// 2초 뒤 레이저 본체 생성
				FTimerHandle Spawn1TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					Spawn1TimerHandle,
					FTimerDelegate::CreateLambda([LocationCopy, RotationCopy, this]()
						{
							// 레이저 생성
							GetWorld()->SpawnActor<ADMLaserZone>(ADMLaserZone::StaticClass(), LocationCopy, RotationCopy);
						}), 2.0f, false);
			}
		}
	}
}

void ADMCharacterPlayer::OnGameStarted()
{
	UE_LOG(LogTemp, Warning, TEXT("Character Received game start notification."));

	// 게임 시작 시 3초 후에 SetDead 호출
	FTimerHandle StartGameTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		StartGameTimerHandle,
		this,
		&ADMCharacterPlayer::SetDead,
		3.0f, false
	);
	// 게임 시작 시 필요한 초기화 작업
	// 예를 들어, UI 업데이트나 초기 상태 설정, 특수 효과 등
}

void ADMCharacterPlayer::SetCharacterControlType(ECharacterControlType NewCharacterControlType)
{
	UE_LOG(LogTemp, Warning, TEXT("Setting CharacterControlType to: %d"), (int32)NewCharacterControlType);
	SetCharacterControl(NewCharacterControlType);
}

void ADMCharacterPlayer::ApplyPlayerMaterial()
{
	if (ADMPlayerController* DMPC = Cast<ADMPlayerController>(GetController()))
	{
		EPlayerRole PlayerRole = DMPC->GetPlayerRole();
		UE_LOG(LogTemp, Warning, TEXT("[Character] ApplyPlayerMaterial: %d"), (int32)PlayerRole);

		if (PlayerRole == EPlayerRole::Player1 && Player1Material)
		{
			GetMesh()->SetMaterial(0, Player1Material);
		}
		else if (PlayerRole == EPlayerRole::Player2 && Player2Material)
		{
			GetMesh()->SetMaterial(0, Player2Material);
		}
	}
}
