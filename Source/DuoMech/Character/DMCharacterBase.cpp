// Fill out your copyright notice in the Description page of Project Settings.


#include "DMCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DMCharacterControlData.h"
#include "DMShootActionData.h"
#include "Physics/DMCollision.h"
#include "Engine/DamageEvents.h"
#include "Item/DMWeaponItemData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/DMAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Gimmick/DMBlockerDoor.h"

// Sets default values
ADMCharacterBase::ADMCharacterBase()
{
	// 폰 기본 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	

	// 캡슐
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_DMCAPSULE);


	// Movement
	// 이동 방향에 따라 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 회전 속도 설정
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	// 점프 시 수직 속도
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	// 공중에서의 이동 제어 비율
	GetCharacterMovement()->AirControl = 1.0f;

	// 추가
	GetCharacterMovement()->BrakingFrictionFactor = 10.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 2048.0f;

	// 기본 걷기 속도
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	//// 지상에서 감속 속도
	//GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	// 최소 걷기 속도
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;


	// 메시
	// 메시 컴포넌트 상대 위치, 회전 설정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -96.0f), FRotator(0.0f, -90.0f, 0.0f));
	// 메시 컴포넌트의 애니메이션 모드 설정
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	// 메시 컴포넌트의 충돌 프로필 설정
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));


	// 메시와 애니메이션 블루프린트 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(
		TEXT("/Game/MikeDoublePack/Robot02/Mesh/SM_Robot02.SM_Robot02"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(
		TEXT("/Game/DuoMech/Animation/ABP_DMCharacter.ABP_DMCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	// Character Control Data 설정
	static ConstructorHelpers::FObjectFinder<UDMCharacterControlData> ForwardDataRef(
		TEXT("/Game/DuoMech/CharacterControl/DMC_Forward.DMC_Forward"));
	if (ForwardDataRef.Object)
	{
		// Character Control Data를 설정
		CharacterControlManager.Add(ECharacterControlType::Forward, ForwardDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UDMCharacterControlData> RightDataRef(
		TEXT("/Game/DuoMech/CharacterControl/DMC_Right.DMC_Right"));
	if (RightDataRef.Object)
	{
		// Character Control Data를 설정
		CharacterControlManager.Add(ECharacterControlType::Right, RightDataRef.Object);
	}

	// Item Section
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ADMCharacterBase::EquipBuster)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ADMCharacterBase::FindKey)));

	// 무기를 보여줄 컴포넌트  생성
	Buster = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Buster"));

	// 메시 컴포넌트 하위로 계층을 설정하고 Buster 소켓에 부착
	Buster->SetupAttachment(GetMesh(), TEXT("Buster"));
}

void ADMCharacterBase::SetCharacterControlData(const UDMCharacterControlData* InCharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = InCharacterControlData->bUseControllerRotationYaw;

	// Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = InCharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = InCharacterControlData->bUseControllerDesireRotation;
	GetCharacterMovement()->RotationRate = InCharacterControlData->RotationRate;
}

void ADMCharacterBase::TakeItem(UDMItemData* InItemData)
{
	TakeItemActions[(uint8)InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
}

void ADMCharacterBase::EquipBuster(UDMItemData* InItemData)
{
	if (bHasBuster)
	{
		return;
	}

	UDMWeaponItemData* WeaponItemData = Cast<UDMWeaponItemData>(InItemData);

	if (WeaponItemData)
	{
		// 아직 무기 메시 로딩이 안된 경우, 로드 처리
		if (WeaponItemData->BusterMesh.IsPending())
		{
			WeaponItemData->BusterMesh.LoadSynchronous();
		}

		// 무기 컴포넌트에 로드가 완료된 스켈레탈 메시 설정
		Buster->SetSkeletalMesh(WeaponItemData->BusterMesh.Get());

		bHasBuster = true;

		if (USkeletalMeshComponent* SkelMesh = GetMesh())
		{
			if (UDMAnimInstance* DMAnimInstance = Cast<UDMAnimInstance>(SkelMesh->GetAnimInstance()))
			{
				DMAnimInstance->bIsCombat = true;
			}
		}
	}
}

void ADMCharacterBase::FindKey(UDMItemData* InItemData)
{
	if (!InItemData)
	{
		return;
	}
}
