#include "ATarzan.h"
#include "Object/StaticMeshComponent/StaticMeshComponent.h"
#include <Object/ActorComponent/Colliders/UBoxCollider.h>
#include <Core/Engine.h>
#include <Object/World/World.h>

ATarzan::ATarzan()
{
	UStaticMeshComponent* MeshComponent = AddComponent<UStaticMeshComponent>();
	RootComponent = MeshComponent;

	MeshComponent->SetStaticMesh(TEXT("Assets/2PX7U16XARLGHIM3W48FS86MJ.obj"));
	//MeshComponent->SetStaticMesh(TEXT("Assets/CheckerSphere.obj"));
	//MeshComponent->SetStaticMesh(TEXT("Assets/Tarzan_textured_MaterialGroups.obj"));

	FTransform Tr = GetActorTransform();
	Tr.Rotate(FVector(-90, 0, 0));
	//Tr.SetScale(FVector(0.3f, 0.3f, 0.3f));


	MeshComponent->SetRelativeTransform(Tr);

	hitCollider = AddComponent<UBoxCollider>();
	UEngine::Get().GetWorld()->AddColliderComponent(hitCollider);
	hitCollider->SetupAttachment(RootComponent);
}

const char* ATarzan::GetTypeName()
{
	return "Tarzan";
}
