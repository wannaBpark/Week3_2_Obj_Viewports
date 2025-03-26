#pragma once
#include "Core/Container/Array.h"
#include "Core/Math/Vector.h"
#include "Core/Rendering/ShaderParameterMacros.h"
#include "ObjImporter.h"
#include "Core/Container/String.h"
#include "Object/NameTypes.h"
#include "Serialization/Archive.h"

struct FNormalVertex
{
	FVector Position = FVector();
	FVector Normal = FVector();
	FVector Tangent = FVector();
	FVector4 Color = FVector4();
	FVector2D UV = FVector2D();

	void Serialize(FArchive& Ar) const
	{
		Ar << Position << Normal << Tangent << Color << UV;
	}
	void Deserialize(FArchive& Ar)
	{
		Ar >> Position >> Normal >> Tangent >> Color >> UV;
	}
};

/*
* 서브메시 정보 구조체
* @GroupName : 서브메시 이름
* @StartIndex : 서브메시 시작 인덱스
* @NumIndices : 서브메시 인덱스 개수
* @MaterialName : 서브메시에 사용될 머티리얼 이름
*/
struct FSubMesh
{
	FName GroupName = "Default";
	uint32 Index = 0;
	uint32 StartIndex = 0;
	uint32 NumIndices = 0;

	//uint32 TextureIndex = 0;
	FName MaterialName = "Default";

	void Serialize(FArchive& Ar) const
	{
		FString MaterialNameDebug = MaterialName.ToString();

		Ar << GroupName << Index << StartIndex << NumIndices << MaterialName;
	}
	void Deserialize(FArchive& Ar)
	{
		Ar >> GroupName >> Index >> StartIndex >> NumIndices >> MaterialName;
	}
};
/*
* 가공된 메시 [실제] 데이터 구조체
* [Preload]타임에 사용될 실제, 그러나 사용되기 좋도록 가공된 데이터
* @Vertices : <정점의 위치, 노말, 색상, 텍스쳐 좌표>
* @Indices : 정점의 인덱스 정보
*/
struct FStaticMesh
{
    FString Name;
	FString Path;

	// 버텍스, 노말, uv, 탄젠트, 버텍스 컬러 정보
    TArray<FNormalVertex> Vertices;
	// 버텍스 인덱스 정보
    TArray<uint32> Indices;
	// 그룹정보
	TArray<FName> GroupNames;

	// Submesh정보
	TMap<FName, FSubMesh> SubMeshes;

	void Serialize(FArchive& Ar) const
	{
		Ar << Name << Path << Vertices << Indices << GroupNames << SubMeshes;
	}

	void Deserialize(FArchive& Ar)
	{
		Ar >> Name >> Path >> Vertices >> Indices >> GroupNames >> SubMeshes;
	}
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
	FName MaterialName;

	FVector4 Ambient;
	FVector4 Diffuse;
	FVector4 Specular;
	FVector Emissive;
	float Shininess;
	float Opacity;
	float OpticalDensity;
	uint32 Illum;
	FName TextureName;
	uint32 TextureMapIndex;
	FString TexturePath;

	void Serialize(FArchive& Ar) const
	{
		FString DebugMaterialName = MaterialName.ToString();

		Ar << MaterialName << Ambient << Diffuse << Specular << Emissive << 
			Shininess << Opacity << OpticalDensity << Illum << TextureName << TextureMapIndex << TexturePath;
	}
	void Deserialize(FArchive& Ar)
	{
		Ar >> MaterialName >> Ambient >> Diffuse >> Specular >> Emissive >>
			Shininess >> Opacity >> OpticalDensity >> Illum >> TextureName >> TextureMapIndex >> TexturePath;
	}
};

