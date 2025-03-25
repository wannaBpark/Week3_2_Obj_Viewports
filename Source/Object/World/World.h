#pragma once
#include "JsonSavehelper.h"
#include "Core/Engine.h"
#include "Core/Container/Array.h"
#include "Core/Container/Set.h"
#include "Core/Container/String.h"
#include "Object/UObject.h"
#include "Debug/DebugConsole.h"
#include "Object/ObjectFactory.h"

#include "Object/Actor/Arrow.h"
#include "Object/Actor/Picker.h"

#include "Object/ActorComponent/Colliders/ICollider.h"

#include "Object/ObjectMacro.h"
#include "Serialization/Archive.h"

enum class EViewModeIndex : uint32
{
	VMI_Lit,
	VMI_Unlit,
	VMI_Wireframe,
};

class AActor;


class UWorld :public UObject
{
	DECLARE_CLASS(UWorld, UObject)
public:
	UWorld() = default;
	virtual ~UWorld() = default;

public:
	void BeginPlay();
	void Tick(float DeltaTime);
	void LateTick(float DeltaTime);

	template <typename T>
		requires std::derived_from<T, AActor>
	T* SpawnActor();
	TArray<AActor*> GetActors() { return Actors; }
  
	bool DestroyActor(AActor* InActor);
	
	void Render();
	void RenderPickingTexture(URenderer& Renderer);
	void DisplayPickingTexture(URenderer& Renderer);
	void RenderMainTexture(URenderer& Renderer);

	void ClearWorld();
	void LoadWorld(const char* SceneName);
	void SaveWorld();

	void AddZIgnoreComponent(UPrimitiveComponent* InComponent);
	void RemoveZIgnoreComponent(UPrimitiveComponent* InComponent) {ZIgnoreRenderComponents.Remove(InComponent); }
	bool ContainsZIgnoreComponent(UPrimitiveComponent* InComponent) {return ZIgnoreRenderComponents.Find(InComponent) != -1; }

	TArray<ICollider*> GetCollidrs() const { return Colliders; }
	void AddColliderComponent(ICollider* InComponent) { Colliders.Add(InComponent); }
	void RemoveColliderComponet(ICollider* InComponent) { Colliders.Remove(InComponent); }
	bool ContainsColliderComponent(ICollider* InComponent) { return Colliders.Find(InComponent) != -1; }
	
	// render
	void AddRenderComponent(class UPrimitiveComponent* Component) { RenderComponents.Add(Component); }
	void RemoveRenderComponent(class UPrimitiveComponent* Component) { RenderComponents.Remove(Component); }

	void SetGridScale(float InGridScale) { GridScale = InGridScale; }
	float GetGridScale() const { return GridScale; }

	void SetViewMode(uint8 InViewMode) { ViewMode = InViewMode; }
	uint32 GetViewMode() const { return ViewMode; }
private:
	UWorldInfo GetWorldInfo() const;

public:
	FString SceneName;
	uint32 Version = 1;
	float GridScale = 1.0f;
	uint8 ViewMode = 0;
	
public:
	// !NOTE : Actor들의 ActorInfo를 받아와서 생성
	void Serialize(FArchive& Ar) const;
	void Deserialize(FArchive& Ar);
	AActor* MakeActor(const FActorInfo& Info);

protected:
	TArray<AActor*> Actors;
	TArray<UPrimitiveComponent*> ZIgnoreRenderComponents;
	TArray<ICollider*> Colliders;
	TArray<AActor*> ActorsToSpawn;
	TArray<AActor*> PendingDestroyActors; // TODO: 추후에 TQueue로 변경
	TSet<UPrimitiveComponent*> RenderComponents;
};

template <typename T>
	requires std::derived_from<T, AActor>
T* UWorld::SpawnActor()
{
	T* Actor = FObjectFactory::ConstructObject<T>();
	
	if (UWorld* World = UEngine::Get().GetWorld())
	{
		Actor->SetWorld(World);
		Actors.Add(Actor);
		ActorsToSpawn.Add(Actor);
		return Actor;
	}

	UE_LOG("Actor Construction Failed. World is nullptr");
	return nullptr;
}