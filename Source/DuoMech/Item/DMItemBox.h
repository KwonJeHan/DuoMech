// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gimmick/DMBlockerDoor.h"
#include "Interface/DMStageResettableInterface.h"
#include "DMItemBox.generated.h"

UCLASS()
class DUOMECH_API ADMItemBox : public AActor, public IDMStageResettableInterface
{
	GENERATED_BODY()
	
public:	
	ADMItemBox();

protected:
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDestroyBox();

	virtual void ResetStage() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCResetItemBoxState();

protected:
	// 액터 충돌 담당 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;

	// 아이템 박스를 보여줄 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	// 박스 상호작용 나이아가라 이펙트
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UNiagaraComponent> WeaponEffect;

	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UNiagaraComponent> KeyEffect;

	// 아이템 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TObjectPtr<class UDMItemData> Item;

public:
	UPROPERTY(EditAnywhere, Replicated, Category = Box)
	TObjectPtr<class ADMBlockerDoor> LinkedDoor;
	
};
