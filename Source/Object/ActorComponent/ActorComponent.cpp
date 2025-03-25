#include "ActorComponent.h"
#include "Object/Actor/Actor.h"
#include "Static/FEditorManager.h"

void UActorComponent::BeginPlay()
{
}

void UActorComponent::Tick(float DeltaTime)
{
}

void UActorComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	if (FEditorManager::Get().GetSelectedComponent() == this)
	{
		FEditorManager::Get().SelectComponent(nullptr);
	}
}

AActor* UActorComponent::GetOwner() const
{
	return Owner;
}
