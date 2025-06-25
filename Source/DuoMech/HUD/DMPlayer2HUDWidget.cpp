// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DMPlayer2HUDWidget.h"
#include "Components/Image.h"

UDMPlayer2HUDWidget::UDMPlayer2HUDWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UDMPlayer2HUDWidget::SetKeyVisualState_Implementation(EInputDirection Direction, bool bPressed)
{
    // 이미 같은 상태면 처리 생략
    if (KeyStates.Contains(Direction) && KeyStates[Direction] == bPressed)
    {
        return;
    }

    // 상태 업데이트
    KeyStates.Add(Direction, bPressed);

    const FLinearColor ActiveColor = FLinearColor::Gray;
    const FLinearColor InactiveColor = FLinearColor::White;

    switch (Direction)
    {
    case EInputDirection::Left:
        if (LeftImage)
        {
            if (bPressed)
            {
                LeftImage->SetColorAndOpacity(ActiveColor);
            }
            else
            {
                LeftImage->SetColorAndOpacity(InactiveColor);
            }
        }
        break;
    case EInputDirection::Right:
        if (RightImage)
        {
            if (bPressed)
            {
                RightImage->SetColorAndOpacity(ActiveColor);
            }
            else
            {
                RightImage->SetColorAndOpacity(InactiveColor);
            }
        }
        break;
    }
}
