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
	// ���� �浹 ��� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;

	// ������ �ڽ��� ������ �޽� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	// �ڽ� ��ȣ�ۿ� ���̾ư��� ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UNiagaraComponent> WeaponEffect;

	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UNiagaraComponent> KeyEffect;

	// ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TObjectPtr<class UDMItemData> Item;

public:
	UPROPERTY(EditAnywhere, Replicated, Category = Box)
	TObjectPtr<class ADMBlockerDoor> LinkedDoor;
	
};
