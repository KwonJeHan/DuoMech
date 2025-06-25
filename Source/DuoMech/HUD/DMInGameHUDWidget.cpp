// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DMInGameHUDWidget.h"

void UDMInGameHUDWidget::SetKeyVisualState_Implementation(EInputDirection Direction, bool bPressed)
{
	UE_LOG(LogTemp, Log, TEXT("SetKeyVisualState called in base class. Direction: %d, Pressed: %s"),
		static_cast<int32>(Direction), bPressed ? TEXT("true") : TEXT("false"));
}
