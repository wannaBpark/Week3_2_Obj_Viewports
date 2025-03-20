#include "Cylinder.h"

#include <Object/ActorComponent/Colliders/UBoxCollider.h>
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>
#include <Core/Engine.h>
#include <Object/World/World.h>
#include "../EEnum.h"


ACylinder::ACylinder()
{

    bCanEverTick = true;

    UCylinderComp* CylinderComponent = AddComponent<UCylinderComp>();
    RootComponent = CylinderComponent;

    hitCollider = AddComponent<UBoxCollider>();
    UEngine::Get().GetWorld()->AddColliderComponent(hitCollider);
    hitCollider->SetupAttachment(RootComponent);

    FHitColliderInfo hitInfo = UEngine::Get().GetRenderer()->HitColliderInfoMap[EPrimitiveType::EPT_Cylinder];

    //hitCollider->RelativeTransform.SetScale(FVector(hitInfo.maxX - hitInfo.minX, hitInfo.maxY - hitInfo.minY, hitInfo.maxZ - hitInfo.minZ));
    hitCollider->RelativeTransform.SetScale(FVector(0.06, 0.06, 0.5));
    //hitCollider->RelativeTransform.SetPosition(FVector((hitInfo.minX + hitInfo.maxX) / 2, (hitInfo.minY + hitInfo.maxY) / 2, (hitInfo.minZ + hitInfo.maxZ) / 2));
    hitCollider->RelativeTransform.SetPosition(FVector(0, 0, 0.25));
	
    SetActorTransform(FTransform());
}

void ACylinder::BeginPlay()
{
    Super::BeginPlay();
}

void ACylinder::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

const char* ACylinder::GetTypeName()
{
    return "Cylinder";
}

