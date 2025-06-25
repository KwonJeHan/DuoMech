// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DMItemData.h"
#include "DMWeaponItemData.generated.h"

/**
 * 
 */
UCLASS()
class DUOMECH_API UDMWeaponItemData : public UDMItemData
{
	GENERATED_BODY()
	
public:
	UDMWeaponItemData();

	// ������ ���⿡ ���� ���̷�Ż �޽�
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSoftObjectPtr<USkeletalMesh> BusterMesh;
};
