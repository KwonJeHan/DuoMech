// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/DMCharacterItemInterface.h"
#include "DMCharacterBase.generated.h"

// ĳ���� ��Ʈ�� Ÿ�� ENUM
UENUM()
enum class ECharacterControlType : uint8
{
	Forward,
	Right
};

DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UDMItemData* /*InItemData*/);

// ��������Ʈ�� �ټ��� �迭(��)�� �����ϱ� ���� ����ü ����
// ��������Ʈ ��ü�� ���ڷ� ����� �� ���� ������ ���� ����ü ������ �ʿ�
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
	// ���� ����ü�� ������ �� �ִ� �迭
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;
public:
	// ������ ȹ�� �� ȣ��� �Լ�
	UFUNCTION()
	virtual void TakeItem(class UDMItemData* InItemData) override;
protected:
	// ������ �������� ó���� �Լ� ����
	UFUNCTION()
	virtual void EquipBuster(class UDMItemData* InItemData);

	UFUNCTION()
	virtual void FindKey(class UDMItemData* InItemData);

	// ���� ������ ȹ�� �� ����� ���̷�Ż �޽� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Buster;

	bool bHasBuster = false;
};
