// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Game/DMGameState.h"
#include "HUD/DMInGameHUDWidget.h"
#include "DMPlayerController.generated.h"

/**
 * 네트워크 움직임 동기화 담당
 */
UCLASS()
class DUOMECH_API ADMPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADMPlayerController();

	// 플레이어 역할 설정/가져오기
	UFUNCTION(Category = "Player Role")
	void SetPlayerRole(EPlayerRole NewRole);

	UFUNCTION(Category = "Player Role")
	EPlayerRole GetPlayerRole() const { return PlayerRole; }

	// 역할 변경 알림
	UFUNCTION()
	void OnRep_PlayerRole();

	// 캐릭터 컨트롤 타입 업데이트
	void UpdateCharacterControlType();

	// 움직임 입력을 서버로 전송
	UFUNCTION(Server, Reliable)
	void ServerRPCReceiveMovementInput(const FMovementData& MovementData);

	// 동기화된 움직임 적용
	void ApplySynchronizedMovement(const FMovementData& MovementData);

	// 클라이언트 알림 함수
	UFUNCTION(Client, Reliable)
	void ClientGameStarted();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	UDMInGameHUDWidget* GetHUDWidget() const { return ActiveHUDWidget; }

protected:
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;
	virtual void PostNetInit() override;

	virtual void OnPossess(APawn* aPawn) override;
	
private:
	// 플레이어 역할
	UPROPERTY(ReplicatedUsing = OnRep_PlayerRole)
	EPlayerRole PlayerRole;

// HUD Section
protected:
	void CreateAndShowHUD();

	/** 각각의 플레이어에게 할당할 HUD 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UDMInGameHUDWidget> Player1HUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UDMInGameHUDWidget> Player2HUDClass;

	/** 현재 표시 중인 HUD 위젯 */
	UPROPERTY()
	UDMInGameHUDWidget* ActiveHUDWidget;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
