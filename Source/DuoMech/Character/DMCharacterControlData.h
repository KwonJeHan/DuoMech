// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DMCharacterControlData.generated.h"

/**
 * 
 */
UCLASS()
class DUOMECH_API UDMCharacterControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// �⺻�� ������ ���� ������ ����
	UDMCharacterControlData();
	
	UPROPERTY(EditAnywhere, Category = Pawn)
	uint8 bUseControllerRotationYaw : 1;	// ���� ��Ʈ�ѷ��� ȸ���� ������� ����

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint8 bOrientRotationToMovement : 1;	// ���� �̵� ������ ���ϵ��� ȸ������ ����

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint8 bUseControllerDesireRotation : 1; // ���� ��Ʈ�ѷ��� ȸ���� ������ ����

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	FRotator RotationRate;	// ���� ȸ�� �ӵ�

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext; // �⺻ �Է� ���� ���ؽ�Ʈ

	UPROPERTY(EditAnywhere, Category = SpringArm)
	float TargetArmLength; // ī�޶� ���� ����

	UPROPERTY(EditAnywhere, Category = SpringArm)
	FRotator RelativeRotation; // ī�޶� ���� ��� ȸ��

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bUsePawnControlRotation : 1; // ī�޶� ���� ���� ȸ���� ������ ����

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bInheritPitch : 1;	// ī�޶� ���� ��ġ�� ��ӹ����� ����

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bInheritYaw : 1;	// ī�޶� ���� �並 ��ӹ����� ����

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bInheritRoll : 1;	// ī�޶� ���� ���� ��ӹ����� ����

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bDoCollisionTest : 1;	// ī�޶� ���� �浹 �׽�Ʈ�� �������� ����
};
