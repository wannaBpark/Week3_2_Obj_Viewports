#include "ObjManager.h"
#include "Object/Mesh/StaticMesh.h"

FString FObjManager::GetObjName(const FString& PathFileName)
{
    return FString();
}

FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName)
{
    FStaticMesh* MeshData = *ObjStaticMeshMap.Find(PathFileName);

    if (MeshData)
        return MeshData;

	FObjImporter Reader(PathFileName);

    return nullptr;
}
