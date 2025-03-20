#include "Cone.h"

#include <Object/PrimitiveComponent/UPrimitiveComponent.h>
#include <Object/ActorComponent/Colliders/UBoxCollider.h>
#include <Core/Engine.h>
#include <Object/World/World.h>
#include <../EEnum.h>


ACone::ACone()
{

    bCanEverTick = true;

    UConeComp* ConeComponent = AddComponent<UConeComp>();
    RootComponent = ConeComponent;

    hitCollider = AddComponent<UBoxCollider>();
    UEngine::Get().GetWorld()->AddColliderComponent(hitCollider);
    hitCollider->SetupAttachment(RootComponent);

    FHitColliderInfo hitInfo = UEngine::Get().GetRenderer()->HitColliderInfoMap[EPrimitiveType::EPT_Cone];

    hitCollider->RelativeTransform.SetScale(FVector(1, 1, 1));
    //hitCollider->RelativeTransform.SetScale(FVector(hitInfo.maxX - hitInfo.minX, hitInfo.maxY - hitInfo.minY, hitInfo.maxZ - hitInfo.minZ));
    hitCollider->RelativeTransform.SetPosition(FVector(0,0,0.5));
    //hitCollider->RelativeTransform.SetPosition(FVector((hitInfo.minX + hitInfo.maxX) / 2, (hitInfo.minY + hitInfo.maxY) / 2, (hitInfo.minZ + hitInfo.maxZ) / 2));
	
    SetActorTransform(FTransform());
}

void ACone::BeginPlay()
{
    Super::BeginPlay();
}

void ACone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

const char* ACone::GetTypeName()
{
    return "Cone";
}

