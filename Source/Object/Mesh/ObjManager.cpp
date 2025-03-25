#include "ObjManager.h"
#include "UStaticMesh.h"
#include "Core/Container/ObjectIterator.h"
#include "Core/Engine.h"
#include "Object/ObjectFactory.h"
#include "Object/Material/Material.h"
#include "MeshBuilder.h"

TMap<FName, FStaticMesh*> FObjManager::ObjStaticMeshMap;
TMap<FName, FObjMaterialInfo> FObjManager::MaterialMap;

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
		// 실제 생성된 MeshData의 머티리얼 정보를 가리키도록
		MaterialMap.Add(Kvp.first, Kvp.second);
	}

	ObjStaticMeshMap.Add(PathFileName, MeshData);

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

	return StaticMesh;
}

class UMaterial* FObjManager::LoadMaterial(const FName& MaterialName)
{
	for (TObjectIterator<UMaterial> It(UEngine::Get().GObjects.begin(), UEngine::Get().GObjects.end()); It; ++It)
	{
		UMaterial* Material = *It;

		if (Material && Material->GetMaterialName() == MaterialName)
			return Material;
	}

	//FString MatNameDebug = MaterialName.ToString();
	FObjMaterialInfo* MaterialInfoPtr = MaterialMap.Find(MaterialName);
	if (MaterialInfoPtr)
	{
		UMaterial* Material = FObjectFactory::ConstructObject<UMaterial>();
		MaterialInfoPtr->MaterialName = MaterialName;
		Material->SetMaterialInfo(MaterialName, *MaterialInfoPtr);
		return Material;
	};

	return nullptr;
}

void FObjManager::ReleaseResources()
{
	for (auto& Kvp : ObjStaticMeshMap)
	{
		delete Kvp.second;
	};
}
