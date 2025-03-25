#pragma once
#include "Core/EngineTypes.h"
#include "Object/UObject.h"
#include "Object/ObjectMacro.h"


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

	FVector GetActorPosition() const;

	virtual void Destroyed();

protected:
	bool bCanEverTick = true;
	class AActor* Owner = nullptr;
};

