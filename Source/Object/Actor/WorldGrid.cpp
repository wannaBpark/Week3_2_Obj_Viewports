#include "Object/Actor/WorldGrid.h"
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>



AWorldGrid::AWorldGrid()
{

	bCanEverTick = true;

	UWorldGridComp* CubeComponent = AddComponent<UWorldGridComp>();
	RootComponent = CubeComponent;

	CubeComponent->SetRelativeTransform(FTransform());
}

void AWorldGrid::BeginPlay()
{
	Super::BeginPlay();
}

void AWorldGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* AWorldGrid::GetTypeName()
{
	return "WorldGrid";
}

