#include "MeshComponent.h"

void UMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMeshComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UMeshComponent::Render()
{
	
	//UEngine::Get().GetRenderer()->RenderPrimitive(this);
	//Renderer->RenderPrimitive(this);
}
