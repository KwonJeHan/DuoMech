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
	
	// ������ �޽�
	UPROPERTY(VisibleAnywhere, Category = Laser)
	TObjectPtr<class UStaticMeshComponent> LaserMesh;

	// ������ ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Laser)
	TObjectPtr<class UNiagaraComponent> LaserEffect;

	// ��Ʈ ������Ʈ ������ �� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Laser)
	TObjectPtr<class USceneComponent> SceneRoot;


	UFUNCTION()
	void DestroySelf();
};
