// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/DMStageResettableInterface.h"
#include "DMCharacterExploder.generated.h"

UENUM()
enum class EExploderState : uint8
{
	Patrol,
	Detected,
	Chasing,
	Exploding,
	Dead
};

UCLASS()
class DUOMECH_API ADMCharacterExploder : public ACharacter, public IDMStageResettableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADMCharacterExploder();

	// ���� ����Ʈ
	UPROPERTY(EditAnywhere, Category = Explosion)
	TObjectPtr<class UNiagaraSystem> ExplosionEffect;

	//// �þ� ���� �ð�ȭ
	UPROPERTY(VisibleAnywhere, Category = Vision)
	TObjectPtr<class UStaticMeshComponent> VisionConeMesh;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void StartExplosionCountdown();
	void Explode();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCExplodeEffect();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHideVisionCone();

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	void SetState(EExploderState NewState);

	virtual void ResetStage() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCResetExploderState();

protected:
	UPROPERTY(EditAnywhere, Category = Expolsion)
	float ExplosionRadius = 300.0f;

	UPROPERTY(EditAnywhere, Category = Expolsion)
	float ExplosionDelay = 3.0f;

	UPROPERTY(VisibleAnywhere, Category = AI)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY()
	AActor* TargetPlayer;

	UPROPERTY(Replicated)
	EExploderState CurrentState;

	FTimerHandle ExplosionTimerHandle;

// Patrol Section
protected:
	FVector PatrolStartLocation;
	FVector PatrolEndLocation;
	
	bool bGoingToTarget = true;
	bool bIsCurrentlyMoving = false;

	UPROPERTY(EditAnywhere, Category = Patrol)
	float PatrolDistance = 500.0f;

	UPROPERTY(EditAnywhere, Category = Patrol)
	float PatrolAcceptanceRadius = 50.0f;

	// ȸ��
	FRotator TargetRotationAfterPause;
	float RotationInterpSpeed = 5.0f;
	bool bIsRotating = false;

protected:
	// ���� ������ �Ӽ��� ��Ʈ��ũ�� �����ϱ� ���� �Լ�
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
