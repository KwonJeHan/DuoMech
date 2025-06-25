// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DMItemBox.h"
#include "DMItemData.generated.h"

// 아이템 종류 열거형
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
	// 아이템 타입을 지정하는 열거형 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EItemType Type;

	EItemType GetType() { return Type; }

	UPROPERTY()
	ADMItemBox* OwningBox;
};
