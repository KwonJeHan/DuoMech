// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMLaserZone.generated.h"

UCLASS()
class DUOMECH_API ADMLaserZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ADMLaserZone();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// 레이저 메시
	UPROPERTY(VisibleAnywhere, Category = Laser)
	TObjectPtr<class UStaticMeshComponent> LaserMesh;

	// 레이저 이펙트
	UPROPERTY(VisibleAnywhere, Category = Laser)
	TObjectPtr<class UNiagaraComponent> LaserEffect;

	// 루트 컴포넌트 설정용 씬 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Laser)
	TObjectPtr<class USceneComponent> SceneRoot;


	UFUNCTION()
	void DestroySelf();
};
