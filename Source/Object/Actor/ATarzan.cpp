#include "ATarzan.h"
#include "Object/StaticMeshComponent/StaticMeshComponent.h"

ATarzan::ATarzan()
{
	UStaticMeshComponent* MeshComponent = AddComponent<UStaticMeshComponent>();
	RootComponent = MeshComponent;

	MeshComponent->SetStaicMesh(TEXT("Assets/2PX7U16XARLGHIM3W48FS86MJ.obj"));
}
