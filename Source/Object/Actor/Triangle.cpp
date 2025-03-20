#include "Object/Actor/Triangle.h"
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>


ATriangle::ATriangle()
{

	bCanEverTick = true;

	UTriangleComp* CubeComponent = AddComponent<UTriangleComp>();
	RootComponent = CubeComponent;

	CubeComponent->SetRelativeTransform(FTransform());
}

void ATriangle::BeginPlay()
{
	Super::BeginPlay();
}

void ATriangle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* ATriangle::GetTypeName()
{
	return "Triangle";
}

