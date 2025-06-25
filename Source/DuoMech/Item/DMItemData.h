// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DMItemBox.h"
#include "DMItemData.generated.h"

// ������ ���� ������
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon = 0,
	Key
};


/**
 * 
 */
UCLASS()
class DUOMECH_API UDMItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// ������ Ÿ���� �����ϴ� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EItemType Type;

	EItemType GetType() { return Type; }

	UPROPERTY()
	ADMItemBox* OwningBox;
};
