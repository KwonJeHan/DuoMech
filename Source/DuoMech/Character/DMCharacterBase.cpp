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
	// �� �⺻ ����
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	

	// ĸ��
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_DMCAPSULE);


	// Movement
	// �̵� ���⿡ ���� ȸ��
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// ȸ�� �ӵ� ����
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	// ���� �� ���� �ӵ�
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	// ���߿����� �̵� ���� ����
	GetCharacterMovement()->AirControl = 1.0f;

	// �߰�
	GetCharacterMovement()->BrakingFrictionFactor = 10.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 2048.0f;

	// �⺻ �ȱ� �ӵ�
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	//// ���󿡼� ���� �ӵ�
	//GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	// �ּ� �ȱ� �ӵ�
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;


	// �޽�
	// �޽� ������Ʈ ��� ��ġ, ȸ�� ����
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -96.0f), FRotator(0.0f, -90.0f, 0.0f));
	// �޽� ������Ʈ�� �ִϸ��̼� ��� ����
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	// �޽� ������Ʈ�� �浹 ������ ����
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));


	// �޽ÿ� �ִϸ��̼� �������Ʈ ����
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

	// Character Control Data ����
	static ConstructorHelpers::FObjectFinder<UDMCharacterControlData> ForwardDataRef(
		TEXT("/Game/DuoMech/CharacterControl/DMC_Forward.DMC_Forward"));
	if (ForwardDataRef.Object)
	{
		// Character Control Data�� ����
		CharacterControlManager.Add(ECharacterControlType::Forward, ForwardDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UDMCharacterControlData> RightDataRef(
		TEXT("/Game/DuoMech/CharacterControl/DMC_Right.DMC_Right"));
	if (RightDataRef.Object)
	{
		// Character Control Data�� ����
		CharacterControlManager.Add(ECharacterControlType::Right, RightDataRef.Object);
	}

	// Item Section
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ADMCharacterBase::EquipBuster)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ADMCharacterBase::FindKey)));

	// ���⸦ ������ ������Ʈ  ����
	Buster = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Buster"));

	// �޽� ������Ʈ ������ ������ �����ϰ� Buster ���Ͽ� ����
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
		// ���� ���� �޽� �ε��� �ȵ� ���, �ε� ó��
		if (WeaponItemData->BusterMesh.IsPending())
		{
			WeaponItemData->BusterMesh.LoadSynchronous();
		}

		// ���� ������Ʈ�� �ε尡 �Ϸ�� ���̷�Ż �޽� ����
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
