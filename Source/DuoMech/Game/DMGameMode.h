// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DMGameMode.generated.h"

/**
 * 플레이어 매칭 및 관리
 */

UCLASS()
class DUOMECH_API ADMGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADMGameMode();

	UFUNCTION()
	void KillAllPlayersAndStartRespawn();

	UFUNCTION()
	void RespawnAllPlayers();

	FTimerHandle TeamRespawnTimerHandle;

	UFUNCTION()
	ADMStageManager* FindStageManager();

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:

	virtual void PreLogin(
		const FString& Options,
		const FString& Address,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage) override;
	
	APlayerController* Login(
		UPlayer* NewPlayer,
		ENetRole InRemoteRole, 
		const FString& Portal,
		const FString& Options,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage) override;

	virtual void StartPlay() override;

	// 플레이어 매칭을 위한 함수
	virtual void PostLogin(APlayerController* NewPlayer) override;
	// 플레이어 로그아웃 시 호출되는 함수
	virtual void Logout(AController* Exiting) override;

	// 플레이어 할당 및 초기화
	void AssignPlayerRole(APlayerController* PlayerController);

	// 연결된 플레이어 관리
	UPROPERTY()
	TArray<APlayerController*> ConnectedPlayers;

	// 최대 플레이어 수
	UPROPERTY(EditDefaultsOnly, Category = "Game Settings")
	int32 MaxPlayers = 2;

	// 게임 시작 조건 체크
	void CheckGameStartConditions();

	// 모든 플레이어에게 게임 시작 알림
	UFUNCTION(Category = "Game Flow")
	void StartDuoMechGame();
};
