#pragma once
#include "Core/EngineTypes.h"
#include "Object/UObject.h"
#include "Object/ObjectMacro.h"
#include "ActorComponentInfo.h"

class UActorComponent : public UObject
{
public:
	DECLARE_CLASS(UActorComponent, UObject)
	UActorComponent() = default;

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void EndPlay(const EEndPlayReason::Type Reason);

	bool CanEverTick() const { return bCanEverTick; }

	virtual class AActor* GetOwner() const;
	virtual void SetOwner(class AActor* InOwner) { Owner = InOwner; }

	// 액터가 Serialize되어 컴포넌트가 instantiate되고난 후 호출
	virtual void LoadAndConstruct(const FActorComponentInfo Info) = 0;
	// 컴포넌트를 Serialize할 때	사용할 정보를 반환
	virtual FActorComponentInfo GetActorComponentInfo() = 0;


protected:
	bool bCanEverTick = true;
	class AActor* Owner = nullptr;
};

