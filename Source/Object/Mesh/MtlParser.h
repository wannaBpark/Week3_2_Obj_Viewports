#include <iostream>
#include <unordered_map>
#include <fstream>
//#include <DirectXMath.h> // [deprecated] : 테스트용으로 XMVector를 사용했음 
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "Core/Math/Vector.h"

//using namespace DirectX;
using namespace std;
namespace fs = std::filesystem;

struct Material
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

OBJLoader loader;
std::unordered_map<std::string, Material> materials;

/*
* Mtl 확장자 파일을 불러와 Material 속성을 저장합니다
* 상대 경로, 절대 경로 모두 호환되며 파일이 존재하는 경우에 저장합니다
*/
class OBJLoader
{
public:
    bool LoadMTL(const std::string& filename, std::unordered_map<std::string, Material>& materials)
    {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        fs::path mtlPath = fs::path(filename).parent_path();
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
                materials[currentMaterial] = Material();            // newmtl 토큰 파싱 시에만 새로운 Material추가 (이름 = newmtl 다음 토큰)
            }
            else if (token == "Ka")
            {
                iss >> materials[currentMaterial].Ambient.X >> materials[currentMaterial].Ambient.Y >> materials[currentMaterial].Ambient.Z;
                materials[currentMaterial].Ambient.W = 1.0f;
            }
            else if (token == "Kd")
            {
                iss >> materials[currentMaterial].Diffuse.X >> materials[currentMaterial].Diffuse.Y >> materials[currentMaterial].Diffuse.Z;
                materials[currentMaterial].Diffuse.W = 1.0f;
            }
            else if (token == "Ks")
            {
                iss >> materials[currentMaterial].Specular.X >> materials[currentMaterial].Specular.Y >> materials[currentMaterial].Specular.Z;
                materials[currentMaterial].Specular.W = 1.0f;
            }
            else if (token == "Ke")
            {
                iss >> materials[currentMaterial].Emissive.X >> materials[currentMaterial].Emissive.Y >> materials[currentMaterial].Emissive.Z;
            }
            else if (token == "Ns")
            {
                iss >> materials[currentMaterial].Shininess;
            }
            else if (token == "Ni")
            {
                iss >> materials[currentMaterial].OpticalDensity;
            }
            else if (token == "d")
            {
                iss >> materials[currentMaterial].Opacity;
            }
            else if (token == "illum")
            {
                iss >> materials[currentMaterial].Illum;
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
                materials[currentMaterial].TextureName = fullPath.string(); 
            }
        }

        return true;
    }
};

/* 
* @returns: mtl 파일이 잘 불러와졌는지 결과 값을 파일로 내보냅니다
*/
void printMaterialsInfo(const std::unordered_map<std::string, Material>& materials, std::ostream& out)
{
    out << std::fixed << std::setprecision(6);
    for (const auto& [name, material] : materials)
    {
        out << "Material: " << name << std::endl;
        out << "  Ambient: " << material.Ambient.X << " " << material.Ambient.Y << " " << material.Ambient.Z << " " << material.Ambient.W << std::endl;
        out << "  Diffuse: " << material.Diffuse.X << " " << material.Diffuse.Y << " " << material.Diffuse.Z << " " << material.Diffuse.W << std::endl;
        out << "  Specular: " << material.Specular.X << " " << material.Specular.Y << " " << material.Specular.Z << " " << material.Specular.W << std::endl;
        out << "  Emissive: " << material.Emissive.X << " " << material.Emissive.Y << " " << material.Emissive.Z << std::endl;
        out << "  Shininess: " << material.Shininess << std::endl;
        out << "  Opacity: " << material.Opacity << std::endl;
        out << "  Optical Density: " << material.OpticalDensity << std::endl;
        out << "  Illum: " << material.Illum << std::endl;
        out << "  Texture: " << material.TextureName;
        if (fs::exists(material.TextureName))
        {
            out << " (File exists)";
        }
        else
        {
            out << " (File not found)";
        }
        out << std::endl << std::endl;
    }
}

void LoadMtlTest()
{
    if (loader.LoadMTL("tarzan.mtl", materials)) // 파일 경로를 직접 호출해야 합니다
    {
        // 콘솔에 출력
        std::cout << "Materials loaded successfully. Printing to console:" << std::endl;
        printMaterialsInfo(materials, std::cout);

        // 파일에 출력
        std::ofstream outFile("materials_output.txt");
        if (outFile.is_open())
        {
            outFile << "Materials loaded successfully. Printing to file:" << std::endl;
            printMaterialsInfo(materials, outFile);
            outFile.close();
            std::cout << "Materials information has been written to 'materials_output.txt'" << std::endl;
        }
        else
        {
            std::cerr << "Unable to open file for writing." << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to load MTL file." << std::endl;
    }
}
