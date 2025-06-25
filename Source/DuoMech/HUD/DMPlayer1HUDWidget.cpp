// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DMPlayer1HUDWidget.h"
#include "Components/Image.h"

UDMPlayer1HUDWidget::UDMPlayer1HUDWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UDMPlayer1HUDWidget::SetKeyVisualState_Implementation(EInputDirection Direction, bool bPressed)
{
    // �̹� ���� ���¸� ó�� ����
    if (KeyStates.Contains(Direction) && KeyStates[Direction] == bPressed)
    {
        return;
    }

    // ���� ������Ʈ
    KeyStates.Add(Direction, bPressed);

    const FLinearColor ActiveColor = FLinearColor::Gray;
    const FLinearColor InactiveColor = FLinearColor::White;

    switch (Direction)
    {
    case EInputDirection::Up:
        if (UpImage)
        {
            if (bPressed)
            {
				UpImage->SetColorAndOpacity(ActiveColor);
            }
            else
            {
				UpImage->SetColorAndOpacity(InactiveColor);
            }
        }
        break;
    case EInputDirection::Down:
        if (DownImage)
        {
            if (bPressed)
            {
                DownImage->SetColorAndOpacity(ActiveColor);
            }
			else
			{
				DownImage->SetColorAndOpacity(InactiveColor);
			}
        }
        break;
    }
}