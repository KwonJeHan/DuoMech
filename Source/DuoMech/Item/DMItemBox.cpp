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
 	// 컴포넌트 생성
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    WeaponEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponEffect"));
    KeyEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("KeyEffect"));

    // 컴포넌트 계층 설정
    RootComponent = Trigger;
    Mesh->SetupAttachment(Trigger);
    WeaponEffect->SetupAttachment(Trigger);
    KeyEffect->SetupAttachment(Trigger);

    WeaponEffect->SetAutoActivate(false);
    KeyEffect->SetAutoActivate(false);

    // 콜리전 프로파일 설정
    Trigger->SetCollisionProfileName(CPROFILE_DMTRIGGER);
    Mesh->SetCollisionProfileName(TEXT("NoCollision"));

    // 박스 크기 조정
    Trigger->SetBoxExtent(FVector(70.0f, 42.0f, 50.0f));

    // 메시 애셋
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

    // 트리거가 발생하는 다이나믹 델리게이트에 함수 등록
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

    // 아이템이 있으면 캐릭터에 아이템 획득 메시지 전달
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
    // 꽝 상자
    if (!Item)
    {
        // 메시 안보이도록 처리
        Mesh->SetHiddenInGame(true);
        // 액터 콜리전 끄기
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
