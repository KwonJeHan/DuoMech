// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/DMCharacterItemInterface.h"
#include "DMCharacterBase.generated.h"

// 캐릭터 컨트롤 타입 ENUM
UENUM()
enum class ECharacterControlType : uint8
{
	Forward,
	Right
};

DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UDMItemData* /*InItemData*/);

// 델리게이트를 다수의 배열(맵)로 관리하기 위한 구조체 선언
// 델리게이트 자체를 인자로 사용할 수 없기 때문에 래퍼 구조체 선언이 필요
USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()

	FTakeItemDelegateWrapper() {}
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate)
		: ItemDelegate(InItemDelegate)
	{
	}

	FOnTakeItemDelegate ItemDelegate;
};

UCLASS()
class DUOMECH_API ADMCharacterBase : public ACharacter, public IDMCharacterItemInterface
{
	GENERATED_BODY()

public:
	ADMCharacterBase();

protected:
	virtual void SetCharacterControlData(const class UDMCharacterControlData* InCharacterControlData);

	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UDMCharacterControlData*> CharacterControlManager;

// Item Section
protected:
	// 래퍼 구조체를 관리할 수 있는 배열
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;
public:
	// 아이템 획득 시 호출될 함수
	UFUNCTION()
	virtual void TakeItem(class UDMItemData* InItemData) override;
protected:
	// 아이템 종류마다 처리될 함수 선언
	UFUNCTION()
	virtual void EquipBuster(class UDMItemData* InItemData);

	UFUNCTION()
	virtual void FindKey(class UDMItemData* InItemData);

	// 무기 아이템 획득 시 사용할 스켈레탈 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Buster;

	bool bHasBuster = false;
};
