#include "ICollider.h"
#include "Object/Actor/Actor.h"
#include "Core/Engine.h"
#include "Object/World/World.h"

void ICollider::EndPlay(const EEndPlayReason::Type Reason)
{
	UEngine::Get().GetWorld()->RemoveColliderComponet(this);
}
