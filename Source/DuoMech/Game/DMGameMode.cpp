// Fill out your copyright notice in the Description page of Project Settings.


#include "DMGameMode.h"
#include "DMGameState.h"
#include "Character/DMCharacterPlayer.h"
#include "Player/DMPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


ADMGameMode::ADMGameMode()
{
	// DefaultPawnClass 설정
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(
		TEXT("/Script/DuoMech.DMCharacterPlayer"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}
	// PlayerControllerClass 설정
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(
		TEXT("/Script/DuoMech.DMPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	// GameStateClass 설정
	static ConstructorHelpers::FClassFinder<AGameStateBase> GameStateClassRef(
		TEXT("/Script/DuoMech.DMGameState"));
	if (GameStateClassRef.Class)
	{
		GameStateClass = GameStateClassRef.Class;
	}
}

void ADMGameMode::PreLogin(
	const FString& Options, 
	const FString& Address, 
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

APlayerController* ADMGameMode::Login(
	UPlayer* NewPlayer, 
	ENetRole InRemoteRole, 
	const FString& Portal,
	const FString& Options, 
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	return NewPlayerController;
}

void ADMGameMode::StartPlay()
{
	Super::StartPlay();
}

void ADMGameMode::KillAllPlayersAndStartRespawn()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = It->Get())
		{
			if (ADMCharacterPlayer* Player = Cast<ADMCharacterPlayer>(PlayerController->GetPawn()))
			{
				Player->SetDead();
			}
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		TeamRespawnTimerHandle,
		this,
		&ADMGameMode::RespawnAllPlayers, 
		5.0f, false);
}

void ADMGameMode::RespawnAllPlayers()
{
	if (ADMStageManager* Stage = FindStageManager()) // 또는 GameState 등에서 참조
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (ADMCharacterPlayer* Player = Cast<ADMCharacterPlayer>(PC->GetPawn()))
				{
					Stage->RespawnPlayer(Player); // 기존 구조 활용
				}
			}
		}
	}
}

ADMStageManager* ADMGameMode::FindStageManager()
{
	for (TActorIterator<ADMStageManager> It(GetWorld()); It; ++It)
	{
		return *It; // 하나만 있다고 가정
	}
	return nullptr;
}

AActor* ADMGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// 서버/클라이언트 구분
	FName DesiredTag;
	if (Player->IsLocalController())
	{
		DesiredTag = "Player1";
	}
	else
	{
		DesiredTag = "Player2";
	}

	// 태그 기반 PlayerStart 검색
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (It->Tags.Contains(DesiredTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("Chose PlayerStart: %s"), *DesiredTag.ToString());
			return *It;
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void ADMGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	// NetDriver 가져오기
	UNetDriver* NetDriver = GetNetDriver();

	if (ConnectedPlayers.Num() < MaxPlayers)
	{
		ConnectedPlayers.Add(NewPlayer);
		AssignPlayerRole(NewPlayer);

		UE_LOG(LogTemp, Warning, TEXT("Player Connected. Total Players : %d/%d"), ConnectedPlayers.Num(), MaxPlayers);
		CheckGameStartConditions();
	}
	else
	{
		// 최대 플레이어 수 초과 시, 연결 거부
		UE_LOG(LogTemp, Warning, TEXT("Game is full. Rejecting player Connection"));
		NewPlayer->ClientTravel(TEXT(""), TRAVEL_Absolute);
	}
}

void ADMGameMode::Logout(AController* Exiting)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Exiting))
	{
		// 플레이어 컨트롤러가 연결된 플레이어 목록에서 제거
		ConnectedPlayers.Remove(PlayerController);

		UE_LOG(LogTemp, Warning, TEXT("Player Disconnected. Remaining Players : %d/%d"), ConnectedPlayers.Num(), MaxPlayers);
	}

	Super::Logout(Exiting);
}

void ADMGameMode::AssignPlayerRole(APlayerController* PlayerController)
{
	// 플레이어 컨트롤러가 유효한지 확인
	if (!PlayerController)
	{
		return;
	}

	// DMPlayerController로 캐스팅
	ADMPlayerController* DMPlayerController = Cast<ADMPlayerController>(PlayerController);
	if (!DMPlayerController)
	{
		return;
	}

	// 플레이어 인덱스에 따라 역할 담당
	int32 PlayerIndex = ConnectedPlayers.Num() - 1; // 현재 플레이어의 인덱스

	if (PlayerIndex == 0)
	{
		// Player1 : Forward/Backward 움직임 담당
		DMPlayerController->SetPlayerRole(EPlayerRole::Player1);
		UE_LOG(LogTemp, Log, TEXT("Assigned Player1 Role Forward/Backward"));
	}
	else if (PlayerIndex == 1)
	{
		// Player2 : Left/Right 움직임 담당
		DMPlayerController->SetPlayerRole(EPlayerRole::Player2);
		UE_LOG(LogTemp, Log, TEXT("Assigned Player2 Role Left/Right"));
	}

	// GameState에 플레이어 등록
	if (ADMGameState* DMGameState = GetGameState<ADMGameState>())
	{
		DMGameState->RegisterPlayer(DMPlayerController);
	}

	// 약간의 지연 시간을 설정하고 캐릭터 컨트롤 타입 설정
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[DMPlayerController]()
		{
			if (IsValid(DMPlayerController))
			{
				DMPlayerController->UpdateCharacterControlType();
			}
		}, 0.1f, false);
}

void ADMGameMode::CheckGameStartConditions()
{
	if (ConnectedPlayers.Num() >= MaxPlayers)
	{
		UE_LOG(LogTemp, Warning, TEXT("All players connected. Starting DuoMech game"));
		// 모든 플레이어가 연결되었을 때 게임 시작
		StartDuoMechGame();
	}
}

void ADMGameMode::StartDuoMechGame()
{
	// GameState를 통해 모든 플레이어에게 게임 시작 알림
	if (ADMGameState* DMGameState = GetGameState<ADMGameState>())
	{
		DMGameState->StartGame();
	}

	// 추가적인 게임 시작 로컬 필요 시 여기 구현
	UE_LOG(LogTemp, Warning, TEXT("DuoMech game started!"));
}
