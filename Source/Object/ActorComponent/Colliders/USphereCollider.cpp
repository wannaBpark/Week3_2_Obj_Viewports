#include "USphereCollider.h"
#include "Object/Actor/Actor.h"

FVector USphereCollider::GetCenter() const
{
	return GetOwner()->GetActorTransform().GetPosition() + offset;
}
