// Fill out your copyright notice in the Description page of Project Settings.


#include "DMGameState.h"
#include "Player/DMPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"

ADMGameState::ADMGameState()
{
	bGameStarted = false; // 게임 시작 여부 초기화
	bIsAttacking = false; // 공격 중인지 여부 초기화
	PrimaryActorTick.bCanEverTick = false; // Tick 기능 비활성화
}

void ADMGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ADMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMGameState, RegisteredPlayers);
	DOREPLIFETIME(ADMGameState, bGameStarted);
	DOREPLIFETIME(ADMGameState, bIsAttacking);
}

void ADMGameState::RegisterPlayer(ADMPlayerController* PlayerController)
{
	// 유효하지 않은 플레이어 컨트롤러이거나 이미 등록된 경우
	if (!PlayerController || RegisteredPlayers.Contains(PlayerController))
	{
		return;
	}

	RegisteredPlayers.Add(PlayerController);

	UE_LOG(LogTemp, Log, TEXT("Player registered in GameState. Total : %d"), RegisteredPlayers.Num());
}

void ADMGameState::StartGame()
{
	if (bGameStarted)
	{
		return;
	}
	
	bGameStarted = true;

	// 모든 플레이어에게 게임 시작 알림
	for (ADMPlayerController* PlayerController : RegisteredPlayers)
	{
		if (PlayerController)
		{
			PlayerController->ClientGameStarted();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Game Started! All players notified."));
}

void ADMGameState::SetIsAttacking(bool bNewValue)
{
	bIsAttacking = bNewValue;
}

void ADMGameState::MulticastRPCMovementInput_Implementation(const FMovementData& MovementData)
{
	// 모든 클라이언트에서 움직임 입력 처리
	for (ADMPlayerController* PlayerController : RegisteredPlayers)
	{
		if (PlayerController && PlayerController->GetPlayerRole() != MovementData.SourcePlayer)
		{
			// 입력을 보낸 플레이어가 아닌 다른 플레이어에게 움직임 적용
			PlayerController->ApplySynchronizedMovement(MovementData);
		}
	}
}

ADMPlayerController* ADMGameState::GetPlayerControllerByRole(EPlayerRole PlayerRole)
{
	for (ADMPlayerController* PlayerController : RegisteredPlayers)
	{
		if (PlayerController && PlayerController->GetPlayerRole() == PlayerRole)
		{
			return PlayerController;
		}
	}
	return nullptr;
}