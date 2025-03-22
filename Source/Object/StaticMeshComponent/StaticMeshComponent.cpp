#include "StaticMeshComponent.h"

void UStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStaticMeshComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UStaticMeshComponent::Render()
{
	//Super::Render();


	//UEngine::Get().GetRenderer()->RenderPrimitive(this);
	//Renderer->RenderPrimitive(this);
}
