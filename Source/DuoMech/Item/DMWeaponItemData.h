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

	// 제공할 무기에 대한 스켈레탈 메시
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSoftObjectPtr<USkeletalMesh> BusterMesh;
};
