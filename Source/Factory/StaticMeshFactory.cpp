#include "StaticMeshFactory.h"
#include "Object/Mesh/ObjManager.h"
#include <Debug/DebugConsole.h>

bool FStaticMeshFactory::Import(const FString& FilePath)
{
    if (FilePath == FString(".obj"))
    {
		return ProcessObjFile(FilePath);
		UE_LOG("Import %s success", FilePath.c_char());
    }

    UE_LOG("Import %s failed", FilePath.c_char());
    return false;
}

bool FStaticMeshFactory::ProcessObjFile(const FString& FilePath)
{
	FStaticMesh* StaticMesh = FObjManager::LoadObjStaticMeshAsset(FilePath);

    // !TODO : tasset으로 Serialize해서 저장
    if (StaticMesh)
    {
		UE_LOG("Build Mesh from %s success", FilePath.c_char());
		return true;
    }
	UE_LOG("Build Mesh from %s failed", FilePath.c_char());
    return false;
}

bool FStaticMeshFactory::ProcessMtlFile(const FString& FilePath)
{
    return false;
}

void FStaticMeshFactory::CopyTextureFiles(const TArray<FString>& TexturePaths)
{
}
