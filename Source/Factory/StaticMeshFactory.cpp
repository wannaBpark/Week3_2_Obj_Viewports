#include "StaticMeshFactory.h"
#include "Object/Mesh/ObjManager.h"
#include <Debug/DebugConsole.h>
#include <Object/Material/Material.h>
#include <filesystem>
#include <Serialization/FWindowsBinHelper.h>
#include "Assets/AssetDefine.h"
#include <filesystem> 
#include "Static/FEditorManager.h"
#include "Core/Rendering/UI/UI.h"

REGISTER_FACTORY(FStaticMeshFactory, ".obj");

bool FStaticMeshFactory::Import(const FString& FilePath)
{
    if (GetFileExtension(FilePath) != ".obj")
    {
        UE_LOG("FStaticMeshFactory : Wrong File Extension");
        return false;
    }

    if (ProcessObjFile(FilePath))
    {
		UE_LOG("FStaticMeshFactory : Import %s success", FilePath.c_char());

		return true;
    }
    UE_LOG("FStaticMeshFactory : Import %s failed", FilePath.c_char());
    return false;
}

bool FStaticMeshFactory::ProcessObjFile(const FString& FilePath)
{
    TArray<FObjMaterialInfo> Materials;
    FStaticMesh* StaticMesh = FObjManager::ImportObjStaticMeshAsset(FilePath, &Materials);

    bool bMeshProcessSuccess = StaticMesh != nullptr;
    bool bMtlProcessSuccess = ProcessMtlFile(Materials);

    if (!bMeshProcessSuccess)
    {
        UE_LOG("Build Mesh from %s failed", FilePath.c_char());
        return false;
    }
	SaveStaticMeshAsset(FilePath, *StaticMesh);

    if (!bMtlProcessSuccess)
    {
		UE_LOG("Build Material from %s failed", FilePath.c_char());
        return false;
    }

	UEngine::Get().GetUI().OnStaticMeshImported(StaticMesh);
    return true;
}

bool FStaticMeshFactory::ProcessMtlFile(TArray<FObjMaterialInfo>& BuiltMaterials)
{

	// 1. FObjMaterialInfo::TexturePath에 Texture가 존재한다면 복사해서 Assets/Textures에 넣는다
    for (FObjMaterialInfo& MaterialInfo : BuiltMaterials)
    {
        FString NewPath = CopyTextureFile(MaterialInfo.TexturePath);
		// 텍스쳐를 복사한 후, 해당 ObjMaterialInfo의 TexturePath를 변경한다
        MaterialInfo.TexturePath = NewPath;
    }

    // 2. FObjMaterialInfo들을 Serialize하여 Assets/Materials에 tasset 파일포맷으로 집어넣는다
	for (const FObjMaterialInfo& MaterialInfo : BuiltMaterials)
	{
		SaveMaterialAsset(MaterialInfo);
	}
    return true;
}

// @return : 복사된 텍스쳐의 경로
FString FStaticMeshFactory::CopyTextureFile(const FString& TexturePaths)
{
	namespace fs = std::filesystem;

	try
	{
        fs::path SrcPath = TexturePaths.c_char();
		fs::path DestinationPath = fs::path(ASSET_DEFAULT_TEXTURE_PATH) / SrcPath.filename();

		fs::create_directories(DestinationPath.parent_path());

        fs::copy_file(SrcPath, DestinationPath, fs::copy_options::overwrite_existing);

		UE_LOG("Copy Texture %s to %s", TexturePaths.c_char(), DestinationPath.string().c_str());
		return FString(DestinationPath.string());
	}
    catch (const fs::filesystem_error& error)
    {
		UE_LOG("Copy Texture %s failed : %s", TexturePaths.c_char(), error.what());
        return FString();
    }
}

void FStaticMeshFactory::SaveStaticMeshAsset(const FString& FilePath, FStaticMesh& StaticMesh)
{
	// filepath에서 확장자 제거하고 이름만 가져오기
	std::filesystem::path Path(FilePath.c_char());
	FString FileName = FString(Path.stem().string());

    FString NewPath = ASSET_DEFAULT_STATICMESH_PATH + FileName + ASSET_DEFAULT_STATICMESH_EXTENSION;
    StaticMesh.Path = NewPath;
	FArchive MeshArchive;
	MeshArchive << StaticMesh;
    
	FWindowsBinHelper::SaveToBin(NewPath, MeshArchive);
}

void FStaticMeshFactory::SaveMaterialAsset(const FObjMaterialInfo& BuiltMaterial)
{
    FArchive MaterialArchive;
	MaterialArchive << BuiltMaterial;

	FString MaterialStr = BuiltMaterial.MaterialName.ToString();

	FWindowsBinHelper::SaveToBin(ASSET_DEFAULT_MATERIAL_PATH + MaterialStr + ASSET_DEFAULT_MATERIAL_EXTENSION, MaterialArchive);
}

