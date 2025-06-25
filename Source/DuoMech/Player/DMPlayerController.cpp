// Fill out your copyright notice in the Description page of Project Settings.


#include "DMPlayerController.h"
#include "Character/DMCharacterPlayer.h"
#include "Game/DMGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SceneViewport.h"
#include "Framework/Application/SlateApplication.h"

ADMPlayerController::ADMPlayerController()
{
	// 초기 역할 설정
	PlayerRole = EPlayerRole::None;

	// HUD 클래스 로딩
	static ConstructorHelpers::FClassFinder<UUserWidget> P1HUD(
		TEXT("/Game/DuoMech/HUD/WBP_DMPlayer1HUD.WBP_DMPlayer1HUD_C"));
	if (P1HUD.Class)
	{
		Player1HUDClass = P1HUD.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> P2HUD(
		TEXT("/Game/DuoMech/HUD/WBP_DMPlayer2HUD.WBP_DMPlayer2HUD_C"));
	if (P2HUD.Class)
	{
		Player2HUDClass = P2HUD.Class;
	}
}

void ADMPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 플레이어 역할을 네트워크로 동기화
	DOREPLIFETIME(ADMPlayerController, PlayerRole);
}

void ADMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("DMPlayerController BeginPlay - Role : %d"), (int32)PlayerRole);

	// 클라이언트에서 폰이 준비되었을 때 캐릭터 컨트롤 타입 설정
	if (!HasAuthority() && GetPawn())
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[this]()
			{
				UpdateCharacterControlType();
			}, 0.5f, false);
	}
}

void ADMPlayerController::CreateAndShowHUD()
{
	if (ActiveHUDWidget)
	{
		ActiveHUDWidget->RemoveFromParent();
		ActiveHUDWidget = nullptr;
	}

	TSubclassOf<UUserWidget> HUDClassToUse = nullptr;

	switch (GetPlayerRole())
	{
	case EPlayerRole::Player1:
		HUDClassToUse = Player1HUDClass;
		break;

	case EPlayerRole::Player2:
		HUDClassToUse = Player2HUDClass;
		break;
	default:
		return;
	}

	if (HUDClassToUse)
	{
		ActiveHUDWidget = CreateWidget<UDMInGameHUDWidget>(this, HUDClassToUse);
		if (ActiveHUDWidget)
		{
			ActiveHUDWidget->AddToViewport();

			// 입력 포커스 설정
			FInputModeGameOnly GameOnlyInputMode;
			SetInputMode(GameOnlyInputMode);
			bShowMouseCursor = false;
		}
	}
}

void ADMPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ADMPlayerController::PostNetInit()
{
	Super::PostNetInit();

	UNetDriver* NetDriver = GetNetDriver();
}

void ADMPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void ADMPlayerController::SetPlayerRole(EPlayerRole NewRole)
{
	PlayerRole = NewRole;

	UE_LOG(LogTemp, Warning, TEXT("[DMPC] SetPlayerRole: %d (%s)"), 
		(int32)PlayerRole, HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));

	// 서버에서도 머티리얼 반영을 위해 직접 호출
	if (HasAuthority())
	{
		OnRep_PlayerRole();
	}
}

void ADMPlayerController::OnRep_PlayerRole()
{
	UE_LOG(LogTemp, Warning, TEXT("[DMPC] OnRep_PlayerRole: %d (%s)"),
		(int32)PlayerRole,
		HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));

	if (APawn* LocalPawn = GetPawn())
	{
		if (ADMCharacterPlayer* PlayerCharacter = Cast<ADMCharacterPlayer>(LocalPawn))
		{
			PlayerCharacter->ApplyPlayerMaterial();
		}
	}

	if (IsLocalController())
	{
		CreateAndShowHUD();
	}
}

void ADMPlayerController::UpdateCharacterControlType()
{
	// 내 캐릭터가 유효한지 확인
	if (ADMCharacterPlayer* MyCharacter = Cast<ADMCharacterPlayer>(GetPawn()))
	{
		// 기본값은 Forward
		ECharacterControlType ControlType = ECharacterControlType::Forward;

		switch (PlayerRole)
		{
		case EPlayerRole::Player1:
			// Player1은 Forward/Backward 움직임
			ControlType = ECharacterControlType::Forward;
			UE_LOG(LogTemp, Warning, TEXT("Setting Character Control to Forward for Player1"));
			break;
		
		case EPlayerRole::Player2:
			// Player2는 Left/Right 움직임
			ControlType = ECharacterControlType::Right;
			UE_LOG(LogTemp, Warning, TEXT("Setting Character Control to Right for Player2"));
			break;
		
		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown Player Role, Defaulting to Forward"));
			break;
		}

		MyCharacter->SetCharacterControlType(ControlType);
	}
}

void ADMPlayerController::ServerRPCReceiveMovementInput_Implementation(const FMovementData& MovementData)
{
	// 서버에서 GameState를 통해 모든 클라이언트에 움직임 동기화
	if (ADMGameState* DMGameState = GetWorld()->GetGameState<ADMGameState>())
	{
		// GameState에 움직임 입력 전달
		DMGameState->MulticastRPCMovementInput(MovementData);
	}
}

void ADMPlayerController::ApplySynchronizedMovement(const FMovementData& MovementData)
{
	// 내 캐릭터에게 동기화된 움직임 적용
	if (ADMCharacterPlayer* MyCharacter = Cast<ADMCharacterPlayer>(GetPawn()))
	{
		MyCharacter->ApplyNetworkedMovement(MovementData);
	}
}

void ADMPlayerController::ClientGameStarted_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Game started notification received on client."));

	// 게임 시작 시 클라이언트에서 수행할 작업
	if (ADMCharacterPlayer* MyCharacter = Cast<ADMCharacterPlayer>(GetPawn()))
	{
		MyCharacter->OnGameStarted();
	}
}
