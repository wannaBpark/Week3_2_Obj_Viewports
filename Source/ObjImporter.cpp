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
#include <Factory/Path.h>
namespace fs = std::filesystem;

bool FObjImporter::LoadMTL(const FString& FileAbsolutePath)
{
	std::ifstream file(*FileAbsolutePath);
	fs::path mtlPath = fs::path(*FileAbsolutePath).parent_path();
	if (!file.is_open()) return false;

	std::string currentMaterial;
	std::string line;

    FName CurrentMaterialName;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string token;
		iss >> token;

		if (token == "newmtl")
		{
			iss >> currentMaterial;
            CurrentMaterialName = FName(currentMaterial);
            MaterialsPerGroup[CurrentMaterialName].MaterialName = CurrentMaterialName;
			//MaterialsPerGroup[currentMaterial] = FObjMaterialInfo();            // newmtl 토큰 파싱 시에만 새로운 Material추가 (이름 = newmtl 다음 토큰)
		}
		else if (token == "Ka")
		{
			iss >> MaterialsPerGroup[CurrentMaterialName].Ambient.X >> MaterialsPerGroup[CurrentMaterialName].Ambient.Y >> MaterialsPerGroup[CurrentMaterialName].Ambient.Z;
			MaterialsPerGroup[CurrentMaterialName].Ambient.W = 1.0f;
		}
		else if (token == "Kd")
		{
			iss >> MaterialsPerGroup[CurrentMaterialName].Diffuse.X >> MaterialsPerGroup[CurrentMaterialName].Diffuse.Y >> MaterialsPerGroup[CurrentMaterialName].Diffuse.Z;
			MaterialsPerGroup[CurrentMaterialName].Diffuse.W = 1.0f;
		}
		else if (token == "Ks")
		{
			iss >> MaterialsPerGroup[CurrentMaterialName].Specular.X >> MaterialsPerGroup[CurrentMaterialName].Specular.Y >> MaterialsPerGroup[CurrentMaterialName].Specular.Z;
			MaterialsPerGroup[CurrentMaterialName].Specular.W = 1.0f;
		}
		else if (token == "Ke")
		{
			iss >> MaterialsPerGroup[CurrentMaterialName].Emissive.X >> MaterialsPerGroup[CurrentMaterialName].Emissive.Y >> MaterialsPerGroup[CurrentMaterialName].Emissive.Z;
		}
		else if (token == "Ns")
		{
			iss >> MaterialsPerGroup[CurrentMaterialName].Shininess;
		}
		else if (token == "Ni")
		{
			iss >> MaterialsPerGroup[CurrentMaterialName].OpticalDensity;
		}
		else if (token == "d")
		{
			iss >> MaterialsPerGroup[CurrentMaterialName].Opacity;
		}
		else if (token == "illum")
		{
			iss >> MaterialsPerGroup[CurrentMaterialName].Illum;
		}
		else if (token == "map_Kd")
		{
            // 이 경로는 Import하는 시점의 texture의 Path
            // 텍스쳐가 mtl과 같은 폴더에 존재할 경우 상대경로, 아니라면 pc의 절대경로가 된다
            // 상대 경로라면, 절대 경로로 수정이 필요하다
            // 즉, Texture를 textures 폴더로 옮긴 후 해당 폴더의 경로로 수정되어야 한다.
			std::string texturePath;
			std::getline(iss >> std::ws, texturePath);

			// 절대 경로로 수정
			fs::path TextureAbsolutePath = mtlPath / fs::path(texturePath);
			//TextureAbsolutePath = fs::canonical(TextureAbsolutePath);
			MaterialsPerGroup[CurrentMaterialName].TexturePath = FString(TextureAbsolutePath.string());

            // 텍스쳐 이름은 경로를 제거하고 pure한 이름만 남긴다
			MaterialsPerGroup[CurrentMaterialName].TextureName = FString(FPath::GetPureFilename(texturePath));
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
    // 원본 obj파일 경로 파싱
    fs::path ObjPath(FilePath.c_char());
    fs::path ObjDirectory = ObjPath.parent_path();
	std::string ObjStem = ObjPath.stem().string();

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
            // !NOTE : Mtl은 Obj와 같은 경로에 있어야 한다!
            // 여기서 같은 경로
            fs::path MtlFileName(Tokens[1]);
			fs::path MtlFilePath = ObjDirectory / MtlFileName;
            MtlFilePath = fs::canonical(MtlFilePath);

			LoadMTL(MtlFilePath.string());
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
			FacesPerGroup.Add(FName(GroupName), TArray<FFaceInfo>());
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
