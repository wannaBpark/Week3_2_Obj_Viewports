#include "Cube.h"
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>
#include <Object/ActorComponent/Colliders/UBoxCollider.h>
#include <Core/Engine.h>
#include <Object/World/World.h>
#include <../EEnum.h>

ACube::ACube()
{

	bCanEverTick = true;

	UCubeComp* CubeComponent = AddComponent<UCubeComp>();
	//UCubeComp* CubeComponent = AddComponent<UCubeComp>();
	RootComponent = CubeComponent;

	hitCollider = AddComponent<UBoxCollider>();
	UEngine::Get().GetWorld()->AddColliderComponent(hitCollider);
	hitCollider->SetupAttachment(RootComponent);
	
	FHitColliderInfo hitInfo = UEngine::Get().GetRenderer()->HitColliderInfoMap[EPrimitiveType::EPT_Cone];

	//hitCollider->RelativeTransform.SetScale(FVector(hitInfo.maxX - hitInfo.minX, hitInfo.maxY - hitInfo.minY, hitInfo.maxZ - hitInfo.minZ));
	//hitCollider->RelativeTransform.SetPosition(FVector((hitInfo.minX + hitInfo.maxX) / 2, (hitInfo.minY + hitInfo.maxY) / 2, (hitInfo.minZ + hitInfo.maxZ) / 2));

	CubeComponent->SetRelativeTransform(FTransform());
}

void ACube::BeginPlay()
{
	Super::BeginPlay();
}

void ACube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* ACube::GetTypeName()
{
	return "Cube";
}

