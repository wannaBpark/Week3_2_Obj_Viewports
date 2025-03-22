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
	TMap<FString, FObjMaterialInfo> GetMaterials() { return Materials; }
	TArray<FString> GetGroupNames() { return GroupNames; }
	TMap<FString, FSubMesh> GetSubMeshes() { return SubMeshes; }


private:
    void MakeVertex(const FVector& Vertex, const FVector& Normal, const FVector2D& UV, FNormalVertex& OutVertex);

    void CalculateTangent(const FNormalVertex& Vertex0, const FNormalVertex& Vertex1, const FNormalVertex& Vertex2, FVector& OutTangent);

    void CreateTextureSRV();
    
    TArray<FNormalVertex> Vertices;
    TArray<uint32> Indices;
	TArray<FString> GroupNames;
    TMap<FString, FObjMaterialInfo> Materials;
	TMap<FString, FSubMesh> SubMeshes;

    uint32 VerticesNum;
    uint32 IndicesNum;
};
