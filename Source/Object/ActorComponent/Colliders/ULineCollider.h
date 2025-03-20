#pragma once
#include "ICollider.h"
#include "Object/ActorComponent/Colliders/CollisionSystem.h";
#include "Object/ObjectMacro.h"

class ULineCollider : public ICollider
{
	DECLARE_CLASS(ULineCollider, ICollider)

public:
	inline FVector GetStart() const { return start; }
	inline FVector GetEnd() const { return end; }
	bool CheckCollision(const ICollider& other) const override {
		return other.CheckCollision(*this);
	}
	
	virtual bool CheckCollision(const class ULineCollider& line) const {
		return CollisionSystem::LineToLine(*this, line);
	}
	virtual bool CheckCollision(const class UBoxCollider& box) const {
		return CollisionSystem::LineToBox(*this, box);
	}
	virtual bool CheckCollision(const class USphereCollider& sphere) const {
		return CollisionSystem::LineToSphere(*this, sphere);
	}

	inline FVector Dir() const {
		FVector result = end - start;
		result.Normalize();
		return result;
	}

private:
	FVector start;
	FVector end;
};

