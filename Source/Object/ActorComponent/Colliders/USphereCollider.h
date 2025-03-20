#pragma once
#include "ICollider.h"
#include "Object/ActorComponent/Colliders/CollisionSystem.h";
#include "Object/ObjectMacro.h"


class USphereCollider : public ICollider
{
	DECLARE_CLASS(USphereCollider, ICollider)

public:
	USphereCollider() {};
	FVector GetCenter() const;
	inline FVector GetOffset() const { return offset; } void SetOffset(FVector value) { offset = value; }
	inline float GetRadius() const { return radius; } void SetRadius(float value) { radius = value; }

	bool CheckCollision(const ICollider& other) const override {
		return other.CheckCollision(*this);
	}

	virtual bool CheckCollision(const class ULineCollider& line) const {
		return CollisionSystem::LineToSphere(line, *this);
	}
	virtual bool CheckCollision(const class UBoxCollider& box) const {
		return CollisionSystem::BoxToSphere(box, *this);
	}
	virtual bool CheckCollision(const class USphereCollider& sphere) const {
		return CollisionSystem::SphereToSphere(*this, sphere);
	}
private:
	FVector offset;
	float radius;
};

