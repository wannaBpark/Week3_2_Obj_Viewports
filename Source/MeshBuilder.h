#pragma once
#include "Core/Container/Array.h"
#include "Core/Container/String.h"
#include "Core/Math/Vector.h"
#include "Object/Mesh/StaticMesh.h"

class FMeshBuilder
{
public:
    FMeshBuilder();

    ~FMeshBuilder();

    bool BuildMeshFromObj(const FString& ObjPath);

    uint32 GetVertexNum() const { return Vertices.Num(); }
    uint32 GetIndexNum() const { return Indices.Num(); }

    TArray<FNormalVertex> GetVertices() { return Vertices; }
    TArray<uint32> GetIndices() { return Indices; }

private:
    void MakeVertex(const TArray<float>& Vertex, const TArray<float>& Normal, const TArray<float>& UV, FNormalVertex& OutVertex);

    void CalculateTangent(const FNormalVertex& Vertex0, const FNormalVertex& Vertex1, const FNormalVertex& Vertex2, FVector& OutTangent);
    
    TArray<FNormalVertex> Vertices;
    TArray<uint32> Indices;

    uint32 VerticesNum;
    uint32 IndicesNum;
};
