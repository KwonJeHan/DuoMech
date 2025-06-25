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
	// 기본값 설정을 위해 생성자 정의
	UDMCharacterControlData();
	
	UPROPERTY(EditAnywhere, Category = Pawn)
	uint8 bUseControllerRotationYaw : 1;	// 폰이 컨트롤러의 회전을 사용할지 여부

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint8 bOrientRotationToMovement : 1;	// 폰이 이동 방향을 향하도록 회전할지 여부

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint8 bUseControllerDesireRotation : 1; // 폰이 컨트롤러의 회전을 따를지 여부

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	FRotator RotationRate;	// 폰의 회전 속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext; // 기본 입력 매핑 컨텍스트

	UPROPERTY(EditAnywhere, Category = SpringArm)
	float TargetArmLength; // 카메라 붐의 길이

	UPROPERTY(EditAnywhere, Category = SpringArm)
	FRotator RelativeRotation; // 카메라 붐의 상대 회전

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bUsePawnControlRotation : 1; // 카메라 붐이 폰의 회전을 따를지 여부

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bInheritPitch : 1;	// 카메라 붐이 피치를 상속받을지 여부

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bInheritYaw : 1;	// 카메라 붐이 요를 상속받을지 여부

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bInheritRoll : 1;	// 카메라 붐이 롤을 상속받을지 여부

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint8 bDoCollisionTest : 1;	// 카메라 붐이 충돌 테스트를 수행할지 여부
};
