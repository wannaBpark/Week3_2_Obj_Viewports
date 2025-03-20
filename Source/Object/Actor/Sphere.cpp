#include "Sphere.h"
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>
#include <Object/ActorComponent/Colliders/UBoxCollider.h>
#include <Core/Engine.h>
#include <Object/World/World.h>

ASphere::ASphere()
{

	bCanEverTick = true;

	USphereComp* SphereComponent = AddComponent<USphereComp>();
	RootComponent = SphereComponent;
	
	hitCollider = AddComponent<UBoxCollider>();
	UEngine::Get().GetWorld()->AddColliderComponent(hitCollider);
	hitCollider->SetupAttachment(RootComponent);
	
	SetActorTransform(FTransform());
}

void ASphere::BeginPlay()
{
	Super::BeginPlay();
}

void ASphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* ASphere::GetTypeName()
{
	return "Sphere";
}

