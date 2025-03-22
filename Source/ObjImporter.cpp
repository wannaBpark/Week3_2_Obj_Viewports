#include "ObjImporter.h"
#include "Core/Container/String.h"
#include "Core/Math/Vector.h"
#include "Object/Mesh/StaticMesh.h"
#include <sstream>
#include <cassert>

FObjImporter::FObjImporter()
{
    FilePath = FString("");
}

FObjImporter::FObjImporter(const FString& InFilePath)
{
    SetFilePath(InFilePath);
}

FObjImporter::~FObjImporter()
{
    Clear();
}

FVector FObjImporter::GetVertex(int32 Idx)
{
    if (0 <= Idx && Idx < Vertices.Num())
    {
        return Vertices[Idx];
    }
    return {};
}

FVector FObjImporter::GetNormal(int32 Idx)
{
    if (0 <= Idx && Idx < Normals.Num())
    {
        return Normals[Idx];
    }
    return {};
}

FVector2D FObjImporter::GetUV(int32 Idx)
{
    if (0 <= Idx && Idx < UVs.Num())
    {
        return UVs[Idx];
    }
    return {};
}

TArray<uint32> FObjImporter::GetVertexIndices()
{
    TArray<uint32> Indices(Vertices.Num() * 3);
    int Cnt = 0;
    for (auto& Kvp : FacesPerGroup)
    {
		for (auto& Face : Kvp.second)
		{
			for (unsigned int i : Face.VertexIndex)
			{
				Indices[Cnt] = i;
				++Cnt;
			}
		}
    }
    
    return Indices;
}

void FObjImporter::SetFilePath(const FString& InFilePath)
{
    if (CheckFile(InFilePath))
    {
        FilePath = InFilePath;
        ReadFile();
    }
}

void FObjImporter::Clear()
{
    Vertices.Empty();

    Normals.Empty();

    UVs.Empty();

    for (auto& kvp : FacesPerGroup)
    {
        kvp.second.Empty();
    }
    FacesPerGroup.Empty();

	MaterialsPerGroup.Empty();
}

TArray<uint32> FObjImporter::ParseFaceInfoPoint(const std::string& line)
{
	std::stringstream ss(line);
	std::string token;
	std::vector<std::string> tokens;


	// ' '를 기준으로 문자열을 분리하여 tokens 벡터에 저장
	while (std::getline(ss, token, '/'))
	{
		tokens.push_back(token);
	}

    TArray<uint32> Infos;

    for (int i = 0; i < tokens.size(); ++i)
    {
		int Value = tokens[i].empty() ? -1 : std::stoi(tokens[i]) - 1;
        Infos.Add(Value);
    }

    //(a/b)형태일 경우 노말이 없는 경우임. -1 추가
	if (Infos.Num() == 2)
	{
		Infos.Add(-1);
	}

	return Infos;
}
#include <filesystem>
namespace fs = std::filesystem;

bool FObjImporter::LoadMTL(const FString& filename)
{
    FString FileName = TEXT("Assets/") + filename;
	std::ifstream file(*FileName);
	if (!file.is_open()) return false;

	fs::path mtlPath = fs::path(*FileName).parent_path();
	std::string currentMaterial;
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string token;
		iss >> token;

		if (token == "newmtl")
		{
			iss >> currentMaterial;
			//MaterialsPerGroup[currentMaterial] = FObjMaterialInfo();            // newmtl 토큰 파싱 시에만 새로운 Material추가 (이름 = newmtl 다음 토큰)
		}
		else if (token == "Ka")
		{
			iss >> MaterialsPerGroup[currentMaterial].Ambient.X >> MaterialsPerGroup[currentMaterial].Ambient.Y >> MaterialsPerGroup[currentMaterial].Ambient.Z;
			MaterialsPerGroup[currentMaterial].Ambient.W = 1.0f;
		}
		else if (token == "Kd")
		{
			iss >> MaterialsPerGroup[currentMaterial].Diffuse.X >> MaterialsPerGroup[currentMaterial].Diffuse.Y >> MaterialsPerGroup[currentMaterial].Diffuse.Z;
			MaterialsPerGroup[currentMaterial].Diffuse.W = 1.0f;
		}
		else if (token == "Ks")
		{
			iss >> MaterialsPerGroup[currentMaterial].Specular.X >> MaterialsPerGroup[currentMaterial].Specular.Y >> MaterialsPerGroup[currentMaterial].Specular.Z;
			MaterialsPerGroup[currentMaterial].Specular.W = 1.0f;
		}
		else if (token == "Ke")
		{
			iss >> MaterialsPerGroup[currentMaterial].Emissive.X >> MaterialsPerGroup[currentMaterial].Emissive.Y >> MaterialsPerGroup[currentMaterial].Emissive.Z;
		}
		else if (token == "Ns")
		{
			iss >> MaterialsPerGroup[currentMaterial].Shininess;
		}
		else if (token == "Ni")
		{
			iss >> MaterialsPerGroup[currentMaterial].OpticalDensity;
		}
		else if (token == "d")
		{
			iss >> MaterialsPerGroup[currentMaterial].Opacity;
		}
		else if (token == "illum")
		{
			iss >> MaterialsPerGroup[currentMaterial].Illum;
		}
		else if (token == "map_Kd")
		{
			std::string texturePath;
			std::getline(iss >> std::ws, texturePath);
			fs::path fullPath = fs::path(texturePath);
			if (fullPath.is_relative())
			{
				fullPath = mtlPath / fullPath;
			}
			MaterialsPerGroup[currentMaterial].TextureName = texturePath;
		}
	}

	return true;
}

bool FObjImporter::CheckFile(const FString& InFilePath) const
{
    std::string path(InFilePath.c_char());
    std::ifstream f(path);
    return f.good();
}

void FObjImporter::ReadFile()
{
    if (!File.is_open())
    {
        File.open(*FilePath);
    }

	FString GroupName = "Default";

    std::string Line; // std::string을 참조해야하므로, FString 대신 std::string 사용
    while (std::getline(File, Line))
    {
        if (Line.empty())
            continue;
        std::stringstream TokenStream(Line);
        TArray<std::string> Tokens;
        std::string Token;
        while (std::getline(TokenStream, Token, ' '))
        {
            Tokens.Add(Token);
        }
        const std::string& Key = Tokens[0];


        // 사용할 머티리얼 파일 로드
        if (Key == "mtllib")
        {
			FString MtlFileName = FString(Tokens[1].c_str());
			LoadMTL(MtlFileName);
        }

        if (Key == "v")
        {
            FVector Vertex
            {
                std::stof(Tokens[1]), // Location X
                std::stof(Tokens[2]), // Location Y
                -std::stof(Tokens[3]) // Location Z
            };
            Vertices.Add(Vertex);
        }
        else if (Key == "vn")
        {
            FVector Normal
            {
                std::stof(Tokens[1]),
                std::stof(Tokens[2]),
                -std::stof(Tokens[3])
            };
            Normals.Add(Normal);
        }
        else if (Key == "vt")
        {
            FVector2D UV
            {
                std::stof(Tokens[1]),      // U
				1.f - std::stof(Tokens[2]) // V; Obj 파일은 오른손 좌표계 기준이므로, 왼손 좌표계의 UV맵 좌표로 변경
            };
            UVs.Add(UV);
        }
        else if (Key == "usemtl")
        {
			GroupName = Tokens[1];
			FacesPerGroup.Add(GroupName, TArray<FFaceInfo>());
        }
        else if (Key == "f")
        {
			FFaceInfo Face;
			for (int i = 0; i < 3; ++i)
			{
				// (a/b/c) 형태의 데이터를 Array로 받아옴
				TArray<uint32> PointInfos = ParseFaceInfoPoint(Tokens[i + 1]);
				Face.VertexIndex[i] = PointInfos[0];
				Face.UVIndex[i] = PointInfos[1];
				Face.NormalIndex[i] = PointInfos[2];
			}
            FacesPerGroup[GroupName].Add(Face);
        }
    }
    File.close();    
}
