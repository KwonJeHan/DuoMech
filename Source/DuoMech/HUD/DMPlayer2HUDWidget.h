// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMInGameHUDWidget.h"
#include "DMPlayer2HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOMECH_API UDMPlayer2HUDWidget : public UDMInGameHUDWidget
{
	GENERATED_BODY()
	
public:
    UDMPlayer2HUDWidget(const FObjectInitializer& ObjectInitializer);

    virtual void SetKeyVisualState_Implementation(EInputDirection Direction, bool bPressed) override;

protected:

    UPROPERTY(meta = (BindWidget))
    class UImage* LeftImage;

    UPROPERTY(meta = (BindWidget))
    class UImage* RightImage;

    TMap<EInputDirection, bool> KeyStates;
};
