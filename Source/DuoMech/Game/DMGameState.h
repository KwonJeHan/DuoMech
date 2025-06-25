// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DMGameState.generated.h"

// 플레이어 역할 정의
UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	None,
	Player1,	// Forward/Backward 움직임 담당
	Player2		// Left/Right 움직임 담당
};

// 움직임 데이터 구조체
USTRUCT()
struct FMovementData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector2D MovementInput;	// 플레이어의 이동 입력 벡터

	UPROPERTY()
	EPlayerRole SourcePlayer;	// 이동 입력의 출처 플레이어 역할

	UPROPERTY()
	bool bIsForwardMovement;	// Forward/Backward 움직임 여부

	FMovementData()
	{
		MovementInput = FVector2D::ZeroVector;
		SourcePlayer = EPlayerRole::None;
		bIsForwardMovement = true; // 기본값은 Forward 움직임
	}

	FMovementData(FVector2D Input, EPlayerRole Player, bool IsForward)
		: MovementInput(Input), SourcePlayer(Player), bIsForwardMovement(IsForward) { }
};

/**
 *  플레이어 상태 및 움직임 동기화 관련
 */

UCLASS()
class DUOMECH_API ADMGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADMGameState();

	// 플레이어 등록
	UFUNCTION()
	void RegisterPlayer(class ADMPlayerController* PlayerController);

	// 게임 시작
	UFUNCTION(Category = "Game Flow")
	void StartGame();

	// 움직임 입력 동기화
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCMovementInput(const FMovementData& MovementData);

	void SetIsAttacking(bool bNewValue);
	bool GetIsAttacking() const { return bIsAttacking; }

	// 플레이어 역할별 컨트롤러 가져오기
	UFUNCTION(Category = "Player Management")
	class ADMPlayerController* GetPlayerControllerByRole(EPlayerRole PlayerRole);

	// 게임 상태 확인
	UFUNCTION(Category = "Game State")
	bool AreAllPlayersConnected() const { return RegisteredPlayers.Num() >= 2; }

protected:
	void BeginPlay();

	// 등록된 플레이어들
	UPROPERTY(Replicated)
	TArray<class ADMPlayerController*> RegisteredPlayers;

	// 게임 시작 여부
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bGameStarted;

	// 공격 중인지 여부
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bIsAttacking;

protected:
	// 게임 상태의 속성을 네트워크로 복제하기 위한 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
