// Raw Data
#include "Core/Math/Vector.h"
#include "Core/Container/Array.h"
#include "Core/Container/String.h"

/*
* 아래 두 구조체는 OBJ 파일의 원시 데이터(Raw Data)를 저장합니다
*/

/*
* 오브젝트 Mesh의 실제 정보를 담고 있는 구조체
* .obj 파일을 읽어서 저장할 때 사용
* .obj 파일은 Vertex, Normal, UV, Index, Material, Texture 정보를 담고 있음
* @TODO: .obj 파일의 정보 확인 후 업데이트 필요
*/
struct FObjInfo
{
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<uint32> VertexIndices;
	TArray<uint32> NormalIndices;
	TArray<uint32> UVIndices;
	TArray<FString> MaterialNames;
	TArray<FString> TextureNames;
    // Vertex List
    // UV List
    // Normal List
    // Vertex Index List
    // UV Index List
    // Normal Index List
    // Material List
    // Texture List
    
};

/*
* 오브젝트 Mesh의 Material 정보를 담고 있는 구조체
* .mtl 파일을 읽어서 저장할 때 사용
* @TODO : .mtl 파일이 어느 정보를 저장하는지 확인 후 업데이트 필요
*/
struct FObjMaterialInfo
{
	FString MaterialName;
	FString TextureName;
	FVector AmbientColor;
	FVector DiffuseColor;
	FVector SpecularColor;
	float SpecularExponent;
	float Transparency;

    // Diffuse Scalar
    // Diffuse Texture
};