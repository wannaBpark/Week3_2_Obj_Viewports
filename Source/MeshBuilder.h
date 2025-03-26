#pragma once
#include "Core/Container/Array.h"
#include "Core/Container/String.h"
#include "Core/Math/Vector.h"
#include "Object/Mesh/StaticMesh.h"
#include <functional>

struct FVertexKey
{
    int PositionIdx;
    int NormalIdx;
    int UVIdx;

    bool operator==(const FVertexKey& Other) const
    {
        return PositionIdx == Other.PositionIdx && NormalIdx == Other.NormalIdx && UVIdx == Other.UVIdx;
    };
};

template<>
struct std::hash<FVertexKey>
{
	std::size_t operator()(const FVertexKey& Key) const
	{
		return ((std::hash<int>()(Key.PositionIdx) ^
			(std::hash<int>()(Key.NormalIdx) << 1)) >> 1) ^
			(std::hash<int>()(Key.UVIdx) << 1);
	}
};

class FMeshBuilder
{
public:
    FMeshBuilder();

    ~FMeshBuilder();

    bool BuildMeshFromObj(const FString& ObjPath);

    uint32 GetVertexNum() const { return Vertices.Num(); }
    uint32 GetIndexNum() const { return Indices.Num(); }

    TArray<struct FNormalVertex> GetVertices() { return Vertices; }
    TArray<uint32> GetIndices() { return Indices; }
	TMap<FName, FObjMaterialInfo> GetMaterials() { return Materials; }
	TArray<FName> GetGroupNames() { return GroupNames; }
	TMap<FName, FSubMesh> GetSubMeshes() { return SubMeshes; }
	TMap<FName, FName> GetMaterialPerGroup() { return MaterialPerGroup; }


private:
    void MakeVertex(const FVector& Vertex, const FVector& Normal, const FVector2D& UV, FNormalVertex& OutVertex);

    void CalculateTangent(const FNormalVertex& Vertex0, const FNormalVertex& Vertex1, const FNormalVertex& Vertex2, FVector& OutTangent);

    void CreateTextureSRV();
    
    TArray<FNormalVertex> Vertices;
    TArray<uint32> Indices;
	TArray<FName> GroupNames;
	// Key : 머티리얼 이름, Value : 머티리얼 정보
    TMap<FName, FObjMaterialInfo> Materials;
	// Key : 그룹 이름, Value : 서브메시 정보
	TMap<FName, FSubMesh> SubMeshes;
	// Key : 그룹 이름, Value : 머티리얼 이름
	TMap<FName, FName> MaterialPerGroup;

    uint32 VerticesNum;
    uint32 IndicesNum;
};
