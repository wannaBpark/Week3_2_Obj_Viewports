#pragma once
#include "Core/HAL/PlatformType.h"
struct FVertexSimple
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
};

enum class EPrimitiveType : uint8
{
	EPT_None,
	EPT_Triangle,
	EPT_Cube,
	EPT_Sphere,
	EPT_Line,
	EPT_Cylinder,
	EPT_Cone,
	EPT_Circle,
	EPT_BillBoard,
	EPT_CubeTex,
	EPT_CylinderTex,
	EPT_TriangleTex,
	EPT_CircleTex,
	EPT_SphereTex,
	EPT_ConeTex,
	EPT_WorldText,
	EPT_BoundingBox,
	EPT_WORLDGRID,
	EPT_SubUV,
	EPT_Max,
};

extern FVertexSimple LineVertices[2];
extern FVertexSimple CubeVertices[36];
extern FVertexSimple SphereVertices[2400];
extern FVertexSimple TriangleVertices[3];
