#pragma once

#include "Core/Math/Vector.h"
#include "ObjImporter.h"

struct FNormalVertex
{
	FVector Position;
	FVector Normal;
	FVector Tangent;
	FVector4 Color;
	FVector2D UV;
};

struct FStaticMesh
{
	FString PathFileName;
};

struct FObjInfo
{

};

struct FObjMaterialInfo
{

};
