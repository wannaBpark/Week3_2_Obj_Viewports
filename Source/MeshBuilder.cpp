#include "MeshBuilder.h"

#include "Object/Mesh/StaticMesh.h"

FMeshBuilder::FMeshBuilder()
    : VerticesNum(0)
    , IndicesNum(0)
{}

FMeshBuilder::~FMeshBuilder()
{
    Vertices.Empty();
    Indices.Empty();
}

bool FMeshBuilder::BuildMeshFromObj(const FString& ObjPath)
{
    FObjImporter Reader(ObjPath);
    
    TArray<TArray<TArray<uint32>>> Faces = Reader.GetFaces();
    
    VerticesNum = Reader.GetFaceNum() * 3;
    IndicesNum = VerticesNum; // 현재는 정보가 중복된 정점을 하나로 합치지 않음.
    
    Vertices = TArray<FNormalVertex>(VerticesNum);
    Indices = TArray<uint32>(IndicesNum);

    uint32 VertexCount = 0;
    for (const TArray<TArray<uint32>>& Face : Faces)
    {
        // Obj 파일은 오른손 좌표계를 따르므로, 왼손 좌표계로 변환하여 저장
        TArray<float> Position;
        TArray<float> UV;
        TArray<float> Normal;
        
        Position = Reader.GetVertex(Face[0][0]);
        UV = Reader.GetUV(Face[0][1]);
        Normal = Reader.GetNormal(Face[0][2]);
        FNormalVertex Vertex0;
        MakeVertex(Position, Normal, UV, Vertex0);

        Position = Reader.GetVertex(Face[2][0]);
        UV = Reader.GetUV(Face[2][1]);
        Normal = Reader.GetNormal(Face[2][2]);
        FNormalVertex Vertex1;
        MakeVertex(Position, Normal, UV, Vertex1);

        Position = Reader.GetVertex(Face[1][0]);
        UV = Reader.GetUV(Face[1][1]);
        Normal = Reader.GetNormal(Face[1][2]);
        FNormalVertex Vertex2;
        MakeVertex(Position, Normal, UV, Vertex2);

        CalculateTangent(Vertex0, Vertex1, Vertex2, Vertex0.Tangent);
        CalculateTangent(Vertex1, Vertex2, Vertex0, Vertex1.Tangent);
        CalculateTangent(Vertex2, Vertex0, Vertex1, Vertex2.Tangent);

        Vertices[VertexCount] = Vertex0;
        Indices[VertexCount] = VertexCount;
        ++VertexCount;

        Vertices[VertexCount] = Vertex1;
        Indices[VertexCount] = VertexCount;
        ++VertexCount;

        Vertices[VertexCount] = Vertex2;
        Indices[VertexCount] = VertexCount;
        ++VertexCount;
    }
    return true;
}

void FMeshBuilder::MakeVertex(const TArray<float>& Vertex, const TArray<float>& Normal, const TArray<float>& UV, 
    FNormalVertex& OutVertex)
{
    OutVertex = {};
    OutVertex.Position = {Vertex[0], Vertex[1], Vertex[2]};
    OutVertex.Normal = {Normal[0], Normal[1], Normal[2]};
    OutVertex.UV = {UV[0], UV[1]};
}

void FMeshBuilder::CalculateTangent(const FNormalVertex& Vertex0, const FNormalVertex& Vertex1,
    const FNormalVertex& Vertex2, FVector& OutTangent)
{
    float s1 = Vertex1.UV.X - Vertex0.UV.X;
    float t1 = Vertex1.UV.Y - Vertex0.UV.Y;
    float s2 = Vertex2.UV.X - Vertex0.UV.X;
    float t2 = Vertex2.UV.Y - Vertex0.UV.Y;
    float E1x = Vertex1.Position.X - Vertex0.Position.X;
    float E1y = Vertex1.Position.Y - Vertex0.Position.Y;
    float E1z = Vertex1.Position.Z - Vertex0.Position.Z;
    float E2x = Vertex2.Position.X - Vertex0.Position.X;
    float E2y = Vertex2.Position.Y - Vertex0.Position.Y;
    float E2z = Vertex2.Position.Z - Vertex0.Position.Z;
    float f = 1.f / (s1 * t2 - s2 * t1);
    float Tx = f * (t2 * E1x - t1 * E2x);
    float Ty = f * (t2 * E1y - t1 * E2y);
    float Tz = f * (t2 * E1z - t1 * E2z);

    OutTangent = FVector(Tx, Ty, Tz);
    OutTangent.Normalize();
}
