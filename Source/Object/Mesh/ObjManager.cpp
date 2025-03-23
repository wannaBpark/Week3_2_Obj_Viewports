#include "ObjManager.h"
#include "UStaticMesh.h"
#include "Core/Container/ObjectIterator.h"
#include "Core/Engine.h"
#include "Object/ObjectFactory.h"
#include "Object/Material/Material.h"
#include "MeshBuilder.h"

TMap<FName, FStaticMesh*> FObjManager::ObjStaticMeshMap;
TMap<FName, FObjMaterialInfo*> FObjManager::MaterialMap;

FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName)
{
    FStaticMesh** MeshDataPtr = ObjStaticMeshMap.Find(PathFileName);

	if (MeshDataPtr)
		return *MeshDataPtr;

    FMeshBuilder MeshBuilder;
    MeshBuilder.BuildMeshFromObj(PathFileName);

	FStaticMesh* MeshData = new FStaticMesh();
	MeshData->PathFileName = PathFileName;
	MeshData->Vertices = MeshBuilder.GetVertices();
	MeshData->Indices = MeshBuilder.GetIndices();
	MeshData->GroupNames = MeshBuilder.GetGroupNames();
	MeshData->SubMeshes = MeshBuilder.GetSubMeshes();

	auto Mats = MeshBuilder.GetMaterials();

	for (auto& Kvp : Mats)
	{
		MaterialMap.Add(Kvp.first, new FObjMaterialInfo(Kvp.second));
	}

    return MeshData;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(const FString& PathFileName)
{
    for (TObjectIterator<UStaticMesh> It(UEngine::Get().GObjects.begin(), UEngine::Get().GObjects.end()); It; ++It)
    {
        UStaticMesh* Mesh = *It;

		if (Mesh && Mesh->GetAssetPathFileName() == PathFileName)
			return Mesh;
    }

	FStaticMesh* MeshAsset = LoadObjStaticMeshAsset(PathFileName);
	UStaticMesh* StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
	StaticMesh->SetStaticMeshAsset(MeshAsset);
	ObjStaticMeshMap.Add(PathFileName, MeshAsset);

	return StaticMesh;
}

UMaterial* FObjManager::LoadMaterial(const FString& MaterialName)
{
	for (TObjectIterator<UMaterial> It(UEngine::Get().GObjects.begin(), UEngine::Get().GObjects.end()); It; ++It)
	{
		UMaterial* Material = *It;

		if (Material && Material->GetMaterialName() == MaterialName)
			return Material;
	}

	// 머티리얼 없음 -> 이럴 경우가 없어야 하는데
	return nullptr;
}
