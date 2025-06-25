// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DMShootActionData.generated.h"

/**
 * 
 */
UCLASS()
class DUOMECH_API UDMShootActionData : public UPrimaryDataAsset
{

	GENERATED_BODY()
	
public:
	UDMShootActionData();

	// ��Ÿ�� ���� �̸� ���λ� (Shoot)
	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix;

	// �ִ� �޺� ���� (1��)
	UPROPERTY(EditAnywhere, Category = Name)
	uint8 MaxComboCount;

	// ������ ��� �ӵ�
	// �ִϸ��̼� �ּ��� �⺻ ��� �ӵ� ��
	UPROPERTY(EditAnywhere, Category = Name)
	float FrameRate;

	// �Է��� ������ ���Դ����� Ȯ���ϴ� ������
	// �߻� �ִϸ��̼� �ּ� ���� ����
	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;
};
