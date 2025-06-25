// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DMItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/DMCollision.h"
#include "Interface/DMCharacterItemInterface.h"
#include "DMItemData.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

#include "Character/DMCharacterBase.h"

// Sets default values
ADMItemBox::ADMItemBox()
{
 	// ������Ʈ ����
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    WeaponEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponEffect"));
    KeyEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("KeyEffect"));

    // ������Ʈ ���� ����
    RootComponent = Trigger;
    Mesh->SetupAttachment(Trigger);
    WeaponEffect->SetupAttachment(Trigger);
    KeyEffect->SetupAttachment(Trigger);

    WeaponEffect->SetAutoActivate(false);
    KeyEffect->SetAutoActivate(false);

    // �ݸ��� �������� ����
    Trigger->SetCollisionProfileName(CPROFILE_DMTRIGGER);
    Mesh->SetCollisionProfileName(TEXT("NoCollision"));

    // �ڽ� ũ�� ����
    Trigger->SetBoxExtent(FVector(70.0f, 42.0f, 50.0f));

    // �޽� �ּ�
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(
        TEXT("/Game/Kobo_Dungeon/Meshes/SM-Chest-01.SM-Chest-01"));
    if (BoxMeshRef.Object)
    {
        Mesh->SetStaticMesh(BoxMeshRef.Object);
    }
    Mesh->AddRelativeLocation(FVector(0.0f, -3.5f, -50.0f));

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> WeaponEffectRef(
        TEXT("/Game/HealPositive/NS/Vfx_AttackUp.Vfx_AttackUp"));
    if (WeaponEffectRef.Object)
    {
        WeaponEffect->SetAsset(WeaponEffectRef.Object);
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> KeyEffectRef(
        TEXT("/Game/NiagaraStylizedFX01/NiagaraSystem/FX_Impack009.FX_Impack009"));
    if (KeyEffectRef.Object)
    {
        KeyEffect->SetAsset(KeyEffectRef.Object);
    }

    // Ʈ���Ű� �߻��ϴ� ���̳��� ��������Ʈ�� �Լ� ���
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADMItemBox::OnOverlapBegin);

    bReplicates = true;
}

void ADMItemBox::BeginPlay()
{
    Super::BeginPlay();

    if (Item)
    {
        Item->OwningBox = this;
    }
}

void ADMItemBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADMItemBox, LinkedDoor);
}

void ADMItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority())
    {
        return;
    }

    // �������� ������ ĳ���Ϳ� ������ ȹ�� �޽��� ����
    IDMCharacterItemInterface* OverlappedPawn = Cast<IDMCharacterItemInterface>(OtherActor);
    if (OverlappedPawn)
    {
        OverlappedPawn->TakeItem(Item);

        if (Item && Item->GetType() == EItemType::Key && LinkedDoor)
        {
            LinkedDoor->MulticastRPCOpen();
        }

        MulticastRPCDestroyBox();
    }
}

void ADMItemBox::MulticastRPCDestroyBox_Implementation()
{
    // �� ����
    if (!Item)
    {
        // �޽� �Ⱥ��̵��� ó��
        Mesh->SetHiddenInGame(true);
        // ���� �ݸ��� ����
        SetActorEnableCollision(false);

        return;
    }

    if (Item->GetType() == EItemType::Weapon)
    {
        WeaponEffect->ActivateSystem();
    }
    else if (Item->GetType() == EItemType::Key)
    {
        KeyEffect->ActivateSystem();
    }
    Mesh->SetHiddenInGame(true);
    SetActorEnableCollision(false);
}

void ADMItemBox::ResetStage()
{
    MulticastRPCResetItemBoxState();
}

void ADMItemBox::MulticastRPCResetItemBoxState_Implementation()
{
    if (Mesh)
    {
        Mesh->SetHiddenInGame(false);
    }

    SetActorEnableCollision(true);

    if (WeaponEffect)
    {
        WeaponEffect->Deactivate();
    }

    if (KeyEffect)
    {
        KeyEffect->Deactivate();
    }
}
