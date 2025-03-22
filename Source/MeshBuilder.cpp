#include "MeshBuilder.h"
#include "Core/Container/Map.h"
#include "Core/Engine.h"

#include <cassert>



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
    // 여기서 바로 읽는다
    FObjImporter Reader(ObjPath);


	// 그룹 이름과 머티리얼 Map을 가져온다
	GroupNames = Reader.GetGroupNames();
	Materials = Reader.GetMaterials();

	CreateTextureSRV();
    
	// 버텍스 및 인덱스 배열 크기 미리 잡는다
   	Vertices = TArray<FNormalVertex>();

	Indices = TArray<uint32>();

	TMap<FVertexKey, uint32> VertexMap;

	// 그룹별로 순회하며 Face 정보를 가져와서 FNormalVertex를 구성한다
    for (const FString& GroupName : GroupNames)
    {
		FSubMesh SubMesh;
		SubMesh.GroupName = GroupName;
		SubMesh.StartIndex = Indices.Num();

		int IndexCount = 0;
		TArray<FFaceInfo> Faces = Reader.GetFaces(GroupName);
		for (const FFaceInfo& FaceInfo : Faces)
		{
			// Obj 파일은 오른손 좌표계를 따르므로, 왼손 좌표계로 변환하여 저장


			FVector Position;
			FVector2D UV;
			FVector Normal;

			int VertexIndex0 = FaceInfo.VertexIndex[0];

			Position = Reader.GetVertex(FaceInfo.VertexIndex[0]);
			UV = Reader.GetUV(FaceInfo.UVIndex[0]);
			Normal = Reader.GetNormal(FaceInfo.NormalIndex[0]);
			FNormalVertex Vertex0;
			MakeVertex(Position, Normal, UV, Vertex0);
			

			int VertexIndex1 = FaceInfo.VertexIndex[1];

			Position = Reader.GetVertex(FaceInfo.VertexIndex[1]);
			UV = Reader.GetUV(FaceInfo.UVIndex[1]);
			Normal = Reader.GetNormal(FaceInfo.NormalIndex[1]);
			FNormalVertex Vertex1;
			MakeVertex(Position, Normal, UV, Vertex1);

			int VertexIndex2 = FaceInfo.VertexIndex[2];
			Position = Reader.GetVertex(FaceInfo.VertexIndex[2]);
			UV = Reader.GetUV(FaceInfo.UVIndex[2]);
			Normal = Reader.GetNormal(FaceInfo.NormalIndex[2]);
			FNormalVertex Vertex2;
			MakeVertex(Position, Normal, UV, Vertex2);

			CalculateTangent(Vertex0, Vertex1, Vertex2, Vertex0.Tangent);
			CalculateTangent(Vertex1, Vertex2, Vertex0, Vertex1.Tangent);
			CalculateTangent(Vertex2, Vertex0, Vertex1, Vertex2.Tangent);

			if (VertexMap.Contains(FVertexKey{ VertexIndex0, FaceInfo.NormalIndex[0], FaceInfo.UVIndex[0] }))
			{
				Indices.Add(VertexMap[FVertexKey{ VertexIndex0, FaceInfo.NormalIndex[0], FaceInfo.UVIndex[0] }]);
			}
			else
			{
				VertexMap.Add(FVertexKey{ VertexIndex0, FaceInfo.NormalIndex[0], FaceInfo.UVIndex[0] }, Vertices.Num());
				Indices.Add(Vertices.Num());
				Vertices.Add(Vertex0);
			};

			if (VertexMap.Contains(FVertexKey{ VertexIndex2, FaceInfo.NormalIndex[2], FaceInfo.UVIndex[2] }))
			{
				Indices.Add(VertexMap[FVertexKey{ VertexIndex2, FaceInfo.NormalIndex[2], FaceInfo.UVIndex[2] }]);
			}
			else
			{
				VertexMap.Add(FVertexKey{ VertexIndex2, FaceInfo.NormalIndex[2], FaceInfo.UVIndex[2] }, Vertices.Num());
				Indices.Add(Vertices.Num());
				Vertices.Add(Vertex2);
			};

			if (VertexMap.Contains(FVertexKey{ VertexIndex1, FaceInfo.NormalIndex[1], FaceInfo.UVIndex[1] }))
			{
				Indices.Add(VertexMap[FVertexKey{ VertexIndex1, FaceInfo.NormalIndex[1], FaceInfo.UVIndex[1] }]);
			}
			else
			{
				VertexMap.Add(FVertexKey{ VertexIndex1, FaceInfo.NormalIndex[1], FaceInfo.UVIndex[1] }, Vertices.Num());
				Indices.Add(Vertices.Num());
				Vertices.Add(Vertex1);
			};

			

			IndexCount += 3;
		}


		SubMesh.NumIndices = IndexCount;
		SubMesh.TextureIndex = Materials[GroupName].TextureMapIndex;
		SubMeshes.Add(GroupName, SubMesh);
    }

    
	// 여기서 텍스쳐 로드해서 ShaderResourceView를 생성
    return true;
}

void FMeshBuilder::MakeVertex(const FVector& Vertex, const FVector& Normal, const FVector2D& UV, 
    FNormalVertex& OutVertex)
{
    OutVertex = {};
    OutVertex.Position = Vertex;
    OutVertex.Normal = Normal;
    OutVertex.UV = UV;
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

#include <codecvt>
void FMeshBuilder::CreateTextureSRV()
{
	for (auto& Material : Materials)
	{
		// 텍스쳐 로드
		std::string texturename = Material.second.TextureName;

		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &texturename[0], (int)texturename.size(), NULL, 0);
		std::wstring str(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &texturename[0], (int)texturename.size(), &str[0], size_needed);

		std::wstring filePath = L"Textures/" + str;
		uint32 index = UEngine::Get().GetRenderer()->CreateTextureSRVW(filePath.c_str());

		Material.second.TextureMapIndex = index;
	}
}
