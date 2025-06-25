// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMEnergyProjectile.generated.h"

UCLASS()
class DUOMECH_API ADMEnergyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADMEnergyProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	// �ݸ��� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Coliision)
	TObjectPtr<class USphereComponent> CollisionComponent;

	// �Ž� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	// ����ü ������
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;
};
