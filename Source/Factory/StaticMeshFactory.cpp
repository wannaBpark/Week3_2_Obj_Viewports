#include "StaticMeshFactory.h"
#include "Object/Mesh/ObjManager.h"
#include <Debug/DebugConsole.h>
#include <Object/Material/Material.h>
#include <filesystem>

bool FStaticMeshFactory::Import(const FString& FilePath)
{
    if (GetFileExtension(FilePath) != "obj")
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
	FStaticMesh* StaticMesh = FObjManager::LoadObjStaticMeshAsset(FilePath, &Materials);

    // !TODO : tasset으로 Serialize해서 저장
	bool bMeshProcessSuccess = StaticMesh != nullptr;
	bool bMtlProcessSuccess = ProcessMtlFile(Materials);

    if (!bMeshProcessSuccess)
    {
		UE_LOG("Build Mesh from %s failed", FilePath.c_char());
		return false;
    }

    if (!bMtlProcessSuccess)
    {
		UE_LOG("Build Material from %s failed", FilePath.c_char());
        return false;
    }

}

bool FStaticMeshFactory::ProcessMtlFile(const TArray<FObjMaterialInfo>& BuiltMaterials)
{
    // 1. FObjMaterialInfo들을 Serialize하여 Assets/Materials에 tasset 파일포맷으로 집어넣는다
    // 
	// 2. FObjMaterialInfo::TexturePath에 Texture가 존재한다면 복사해서 Assets/Textures에 넣는다

    for (const FObjMaterialInfo& MaterialInfo : BuiltMaterials)
    {
        // !TODO : Serialize and Save as .tasset
        CopyTextureFile(MaterialInfo.TexturePath);
    }



    return false;
}

void FStaticMeshFactory::CopyTextureFile(const FString& TexturePath)
{
	namespace fs = std::filesystem;

	try
	{
        fs::path SrcPath = TexturePath.c_char();
		fs::path DestinationPath = fs::path("Assets/Textures") / SrcPath.filename();

		fs::create_directories(DestinationPath.parent_path());

        fs::copy_file(SrcPath, DestinationPath, fs::copy_options::overwrite_existing);

		UE_LOG("Copy Texture %s to %s", TexturePath.c_char(), DestinationPath.string().c_str());
	}
    catch (const fs::filesystem_error& error)
    {
		UE_LOG("Copy Texture %s failed : %s", TexturePath.c_char(), error.what());
    }
}

bool FStaticMeshFactory::SaveAsTAsset(const FString& SavePath)
{
    return false;
}
