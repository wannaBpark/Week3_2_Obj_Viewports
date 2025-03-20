#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/Math/Vector.h"


struct FStaticMeshVertex
{
	FVector Position;
	FVector Normal;
	FVector Tangent;
	FVector2D UV;
};