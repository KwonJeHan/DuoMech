// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMCharacterBase.h"
#include "InputActionValue.h"
#include "Game/DMGameState.h"
#include "DMEnergyProjectile.h"
#include "Gimmick/DMStageManager.h"
#include "DMCharacterPlayer.generated.h"

/**
 * 
 */

UCLASS()
class DUOMECH_API ADMCharacterPlayer : public ADMCharacterBase
{
	GENERATED_BODY()
	
public:
	// 생성자
	ADMCharacterPlayer();

protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	// 게임 시작 시 호출되는 함수
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Owner() override;
	virtual void PostNetInit() override;

public:
	// 플레이어 입력 컴포넌트 설정
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 네트워크 움직임 적용
	void ApplyNetworkedMovement(const FMovementData& MovementData);

	// 게임 시작 알림
	void OnGameStarted();

	// 캐릭터 컨트롤 타입 설정
	UFUNCTION(Category = "Character Control")
	void SetCharacterControlType(ECharacterControlType NewCharacterControlType);

	void ApplyPlayerMaterial();

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	TObjectPtr<class UMaterialInterface> Player1Material;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	TObjectPtr<class UMaterialInterface> Player2Material;

// Character Control Section
protected:
	// 캐릭터 컨트롤 타입 설정
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);

	// 캐릭터 컨트롤 데이터 설정
	virtual void SetCharacterControlData(const class UDMCharacterControlData* InCharacterControlData) override;
	
// Dead Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

	UPROPERTY(ReplicatedUsing = OnRep_CanDie)
	bool bCanDie = true;

	UFUNCTION()
	void OnRep_CanDie();

public:
	UFUNCTION(BlueprintCallable)
	void SetDead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCOnDeath();

	bool bIsDead = false;

	// 죽는 애니메이션 재생 함수
	void PlayDeadAnimation();

	// 죽은 뒤 이벤트 발생 전까지 대기할 시간 값
	float DeadEventDelayTime = 5.0f;

	UFUNCTION()
	void SetCanDie(bool bNewCanDie);

	FTimerHandle RespawnTimerHandle;

	UFUNCTION()
	void RequestRespawn();

	ADMStageManager* FindStageManager();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCResetState();

// Camera Section
protected:
	// 카메라 붐 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	// 플레이어를 따라다니는 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

// Input Section
protected:

	// 기본 입력 매핑 컨텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	// 이동 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ForwardMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RightMoveAction;

	// 발사 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShootAction;

	// 입력 액션 핸들러
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);

	UFUNCTION()
	void MoveStopped(const FInputActionValue& Value);

	// 내부 움직임 처리 함수
	void ProcessMovement(const FVector2D& MovementVector, bool bIsForwardMovement);

	// 현재 캐릭터 컨트롤 타입
	ECharacterControlType CurrentCharacterControlType;

// Shoot Section
	// 발사 몽타주 애셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> ShootMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shoot", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDMShootActionData> ShootActionData;

	UPROPERTY(EditAnywhere, Category = "Shoot")
	TSubclassOf<ADMEnergyProjectile> ProjectileClass;

	void Shoot();

	UFUNCTION(Server, Reliable)
	void ServerRPCShoot();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayShootMontage();

	void SpawnProjectile();

	UFUNCTION()
	void OnShootMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void SetAttackState(bool bAttacking);

protected:
	FTimerHandle Player1LaserTimerHandle;
	FTimerHandle Player2LaserTimerHandle;

	UFUNCTION()
	void SpawnLaserForPlayer1();

	UFUNCTION()
	void SpawnLaserForPlayer2();

public:
	UFUNCTION()
	void SetLaserTickEnabled(bool bActive);

public:
	void UpdateHUDDirectionState(const FVector2D& MovementVector);
	
	// 현재 입력값을 저장
	FVector2D CachedMovementVector = FVector2D::ZeroVector;

};
