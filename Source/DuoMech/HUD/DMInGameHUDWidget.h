// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DMInGameHUDWidget.generated.h"

UENUM(BlueprintType)
enum class EInputDirection : uint8
{
	Up,
	Down,
	Left,
	Right
};

/**
 * 
 */
UCLASS()
class DUOMECH_API UDMInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void SetKeyVisualState(EInputDirection Direction, bool bPressed);

	virtual void SetKeyVisualState_Implementation(EInputDirection Direction, bool bPressed);
};
