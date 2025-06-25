// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMInGameHUDWidget.h"
#include "DMPlayer1HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOMECH_API UDMPlayer1HUDWidget : public UDMInGameHUDWidget
{
	GENERATED_BODY()
	
public:
    UDMPlayer1HUDWidget(const FObjectInitializer& ObjectInitializer);

    virtual void SetKeyVisualState_Implementation(EInputDirection Direction, bool bPressed) override;

protected:

    UPROPERTY(meta = (BindWidget))
    class UImage* UpImage;

    UPROPERTY(meta = (BindWidget))
    class UImage* DownImage;

    TMap<EInputDirection, bool> KeyStates;
};
