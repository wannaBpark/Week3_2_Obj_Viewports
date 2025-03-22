#pragma once
#include "Core/Container/Array.h"
#include "Core/Math/Vector.h"
#include "Core/Rendering/ShaderParameterMacros.h"
#include "ObjImporter.h"
#include "MeshBuilder.h"

struct FNormalVertex
{
    FVector Position;
	FVector Normal;
	FVector Tangent;
	FVector4 Color;
	FVector2D UV;
	bool bUseColor = false;
};

/*
* 가공된 메시 [실제] 데이터 구조체
* [Preload]타임에 사용될 실제, 그러나 사용되기 좋도록 가공된 데이터
* @Vertices : <정점의 위치, 노말, 색상, 텍스쳐 좌표>
* @Indices : 정점의 인덱스 정보
* @TODO: StaticMesh가 어느 정보를 얼마나 담을지 고민 필요
*/
struct FStaticMesh
{
    FString PathFileName;

	// 버텍스, 노말, uv, 탄젠트, 버텍스 컬러 정보
    TArray<FNormalVertex> Vertices;
	// 버텍스 인덱스 정보
    TArray<uint32> Indices;
	// 머티리얼 정보. 텍스쳐 이름 포함
	TMap<FString, FObjMaterialInfo> Materials;
	// 그룹정보
	TArray<FString> GroupNames;
	// 그룹 당 Face 정보
	TMap<FString, TArray<FFaceInfo>> FacesPerGroup;
};

// Obj파일의 Face정보를 저장하는 구조체. UV, Normal의 데이터가 없을 경우에 대비해 -1로 초기화
struct FFaceInfo
{
	int VertexIndex[3] = { -1, -1, -1 };
	int UVIndex[3] = { -1, -1, -1 };
	int NormalIndex[3] = { -1, -1, -1 };
};

// 머티리얼 정보
struct FObjMaterialInfo
{
	FVector4 Ambient;
	FVector4 Diffuse;
	FVector4 Specular;
	FVector Emissive;
	float Shininess;
	float Opacity;
	float OpticalDensity;
	uint32 Illum;
	std::string TextureName;
};

struct FSubMesh
{
	FString GroupName;
	uint32 StartIndex;
	uint32 NumIndices;
};