﻿#include "ActorComponent.h"
#include "Object/Actor/Actor.h"

void UActorComponent::BeginPlay()
{
}

void UActorComponent::Tick(float DeltaTime)
{
}

void UActorComponent::EndPlay(const EEndPlayReason::Type Reason)
{
}

AActor* UActorComponent::GetOwner() const
{
	return Owner;
}
