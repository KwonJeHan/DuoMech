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

	// 몽타주 섹션 이름 접두사 (Shoot)
	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix;

	// 최대 콤보 개수 (1개)
	UPROPERTY(EditAnywhere, Category = Name)
	uint8 MaxComboCount;

	// 프레임 재생 속도
	// 애니메이션 애셋의 기본 재생 속도 값
	UPROPERTY(EditAnywhere, Category = Name)
	float FrameRate;

	// 입력이 사전에 들어왔는지를 확인하는 프레임
	// 발사 애니메이션 애셋 별로 지정
	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;
};
