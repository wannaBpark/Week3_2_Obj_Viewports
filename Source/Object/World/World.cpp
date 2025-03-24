#include "World.h"
#include <cassert>
#include "JsonSavehelper.h"
#include "Core/Container/Map.h"
#include "Core/Input/PlayerInput.h"
#include "Object/Actor/Camera.h"
#include <Object/Gizmo/GizmoHandle.h>
#include "Object/Actor/Cone.h"
#include "Object/Actor/Cube.h"
#include "Object/Actor/Cylinder.h"
#include "Object/Actor/Circle.h"
#include "Object/Actor/Sphere.h"
#include "Object/Actor/WorldGrid.h"
#include "Object/Gizmo/WorldGizmo.h"
#include "Object/Actor/WorldText.h"
#include "Object/Gizmo/Axis.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"
#include "Core/FSceneManager.h" 


void UWorld::BeginPlay()
{
	for (const auto& Actor : Actors)
	{
		Actor->BeginPlay();
	}
}

void UWorld::Tick(float DeltaTime)
{
	for (const auto& Actor : ActorsToSpawn)
	{
		Actor->BeginPlay();
	}
	ActorsToSpawn.Empty();

	const auto CopyActors = Actors;
	for (const auto& Actor : CopyActors)
	{
		if (Actor->CanEverTick())
		{
			Actor->Tick(DeltaTime);
		}
	}
}

void UWorld::LateTick(float DeltaTime)
{
	const auto CopyActors = Actors;
	for (const auto& Actor : CopyActors)
	{
		if (Actor->CanEverTick())
		{
			Actor->LateTick(DeltaTime);
		}
	}

	for (const auto& PendingActor : PendingDestroyActors)
	{
		// Engine에서 제거
		UEngine::Get().GObjects.Remove(PendingActor->GetUUID());
	}
	PendingDestroyActors.Empty();
}

void UWorld::Render()
{
	URenderer* Renderer = UEngine::Get().GetRenderer();

	if (Renderer == nullptr)
	{
		return;
	}

	ACamera* cam = FEditorManager::Get().GetCamera();
	Renderer->UpdateViewMatrix(cam->GetActorTransform());
	Renderer->UpdateProjectionMatrix(cam);
	
	if (APlayerInput::Get().GetMouseDown(false))
	{
		//RenderPickingTexture(*Renderer);
	}
	
	RenderMainTexture(*Renderer);

	
	// DisplayPickingTexture(*Renderer);

}

//void UWorld::RenderPickingTexture(URenderer& Renderer)
//{
//	Renderer.PreparePicking();
//	Renderer.PreparePickingShader();
//
//	for (auto& RenderComponent : RenderComponents)
//	{
//		if (RenderComponent->GetOwner()->GetDepth() > 0)
//		{
//			continue;
//		}
//		uint32 UUID = RenderComponent->GetUUID();
		//RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
//		RenderComponent->Render();
//	}
//
//	Renderer.PrepareZIgnore();
//	for (auto& RenderComponent: ZIgnoreRenderComponents)
//	{
//		uint32 UUID = RenderComponent->GetUUID();
//		RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
//		uint32 depth = RenderComponent->GetOwner()->GetDepth();
//		RenderComponent->Render();
//	}
//}

void UWorld::RenderMainTexture(URenderer& Renderer)
{
	//Renderer.Prepare(); // [Deprecated] 모든 뷰포트가 다 렌더된 후 clearRTV 해야하므로 지움
	uint32 showFlagMask = FSceneManager::Get().GetShowFlagMask();

	for (auto& RenderComponent : RenderComponents)
	{
		uint32 depth = RenderComponent->GetOwner()->GetDepth();
		if (depth > 0)
		{
			continue;
		}

		if ((showFlagMask & EShowFlag::Grid) == 0 && RenderComponent->GetOwner()->GetClass() == AWorldGrid::StaticClass())
		{
			continue; // Grid 렌더링 비활성화
		}
		if ((showFlagMask & EShowFlag::Primitive) == 0 
			&& RenderComponent->GetOwner()->GetClass() != AAxis::StaticClass()
			&& RenderComponent->GetClass() != UBoundingBoxComp::StaticClass())
		{
			continue; // Primitive 렌더링 비활성화
			// 기본적으로 RenderComponent는 PrimitiveComponent이므로
			// AAxis는 예외
			// BoundingBox 예외
			// Text는 AWorldText의 Tick에서 처리
		}
		if ((showFlagMask & EShowFlag::BoundingBox) == 0 && RenderComponent->GetClass() == UBoundingBoxComp::StaticClass())
		{
			continue; // Bounding Box
		}

		uint32 UUID = RenderComponent->GetUUID();
		RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
		RenderComponent->Render();
	}

	Renderer.PrepareZIgnore();
	for (auto& RenderComponent : ZIgnoreRenderComponents)
	{
		uint32 UUID = RenderComponent->GetUUID();
		RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
		RenderComponent->Render();
	}
}

void UWorld::DisplayPickingTexture(URenderer& Renderer)
{
	Renderer.RenderPickingTexture();
}

void UWorld::ClearWorld()
{
	TArray CopyActors = Actors;
	for (AActor* Actor : CopyActors)
	{
		if (!Actor->IsGizmoActor())
		{
			DestroyActor(Actor);
		}
	}
	UE_LOG("Clear World");
}


bool UWorld::DestroyActor(AActor* InActor)
{
	// 나중에 Destroy가 실패할 일이 있다면 return false; 하기
	assert(InActor);

	if (PendingDestroyActors.Find(InActor) != -1)
	{
		return true;
	}

	// 삭제될 때 Destroyed 호출
	InActor->Destroyed();

	// World에서 제거
	Actors.Remove(InActor);

	// 제거 대기열에 추가
	PendingDestroyActors.Add(InActor);
	return true;
}

void UWorld::SaveWorld()
{
	const UWorldInfo& WorldInfo = GetWorldInfo();
	JsonSaveHelper::SaveScene(WorldInfo);
}

void UWorld::AddZIgnoreComponent(UPrimitiveComponent* InComponent)
{
	ZIgnoreRenderComponents.Add(InComponent);
	InComponent->SetIsOrthoGraphic(true);
}

void UWorld::LoadWorld(const char* SceneName)
{
	if (SceneName == nullptr || strcmp(SceneName, "") == 0){
		return;
	}
	
	UWorldInfo* WorldInfo = JsonSaveHelper::LoadScene(SceneName);
	if (WorldInfo == nullptr) return;

	ClearWorld();

	Version = WorldInfo->Version;
	this->SceneName = WorldInfo->SceneName;
	uint32 ActorCount = WorldInfo->ActorCount;

	// Type 확인
	for (uint32 i = 0; i < ActorCount; i++)
	{
		UObjectInfo* ObjectInfo = WorldInfo->ObjctInfos[i];
		FTransform Transform = FTransform(ObjectInfo->Location, FQuat(), ObjectInfo->Scale);
		Transform.Rotate(ObjectInfo->Rotation);

		AActor* Actor = nullptr;

		//UClass* ClassInfo = UClass::GetClass(ObjectInfo->ObjectType);
		
		if (ObjectInfo->ObjectType == "Actor")
		{
			Actor = SpawnActor<AActor>();
		}
		else if (ObjectInfo->ObjectType == "Sphere")
		{
			Actor = SpawnActor<ASphere>();
		}
		else if (ObjectInfo->ObjectType == "Cube")
		{
			Actor = SpawnActor<ACube>();
		}
		else if (ObjectInfo->ObjectType == "Arrow")
		{
			Actor = SpawnActor<AArrow>();
		}
		else if (ObjectInfo->ObjectType == "Cylinder")
		{
			Actor = SpawnActor<ACylinder>();
		}
		else if (ObjectInfo->ObjectType == "Cone")
		{
			Actor = SpawnActor<ACone>();
		}
		else if (ObjectInfo->ObjectType == "Circle")
		{
			Actor = SpawnActor<ACircle>();
		}
		else if (ObjectInfo->ObjectType == "WorldText")
		{
			Actor = SpawnActor<AWorldText>();
		}
		// !TODO : 추가된 액터에대한 Spawn로직 추가
		else
		{
			UE_LOG("Unknown Actor Type");
			continue;
		}

		if(Actor)
			Actor->SetActorTransform(Transform);
	}
}

UWorldInfo UWorld::GetWorldInfo() const
{
	UWorldInfo WorldInfo;
	WorldInfo.ActorCount = Actors.Num();
	WorldInfo.ObjctInfos = new UObjectInfo*[WorldInfo.ActorCount];
	WorldInfo.SceneName = *SceneName;
	WorldInfo.Version = 1;
	uint32 i = 0;
	for (auto& actor : Actors)
	{
		if (actor->IsGizmoActor())
		{
			WorldInfo.ActorCount--;
			continue;
		}
		WorldInfo.ObjctInfos[i] = new UObjectInfo();
		const FTransform& Transform = actor->GetActorTransform();
		WorldInfo.ObjctInfos[i]->Location = Transform.GetPosition();
		WorldInfo.ObjctInfos[i]->Rotation = Transform.GetRotation();
		WorldInfo.ObjctInfos[i]->Scale = Transform.GetScale();
		const char* ActorName = actor->GetTypeName();
		WorldInfo.ObjctInfos[i]->ObjectType = ActorName;

		WorldInfo.ObjctInfos[i]->UUID = actor->GetUUID();
		i++;
	}
	return WorldInfo;
}