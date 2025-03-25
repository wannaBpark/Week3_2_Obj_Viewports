#pragma once
#include "Core/Math/Vector.h";
#include "Object/USceneComponent.h"
#include "Core/Math/Transform.h"
#include "Object/ObjectMacro.h"

class ICollider : public USceneComponent
{
public:
	DECLARE_CLASS(ICollider, USceneComponent)
	virtual ~ICollider() = default;

	inline bool GetIsTrigger() const { return bIsTrigger; }

	virtual bool CheckCollision(const ICollider& other) const = 0;
	virtual bool CheckCollision(const class ULineCollider& line) const = 0;
	virtual bool CheckCollision(const class UBoxCollider& box) const = 0;
	virtual bool CheckCollision(const class USphereCollider& sphere) const = 0;

	virtual void EndPlay(const EEndPlayReason::Type Reason);

private:
	bool bIsTrigger = false;
};

