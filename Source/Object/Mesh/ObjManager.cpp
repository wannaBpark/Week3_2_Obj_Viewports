#include "ObjManager.h"
#include "MeshBuilder.h"
#include "UStaticMesh.h"
#include "Core/Engine.h"
#include "Object/UObjectIterator.h"
#include "Object/ObjectFactory.h"
#include "Object/Material/Material.h"

TMap<FName, FStaticMesh*> FObjManager::ObjStaticMeshMap;
TMap<FName, FObjMaterialInfo> FObjManager::MaterialMap;

FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName, TArray<FObjMaterialInfo>* OutMaterials)
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
		if (OutMaterials)
		{
			OutMaterials->Add(Kvp.second);
		}
	}

	ObjStaticMeshMap.Add(PathFileName, MeshData);

    return MeshData;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(const FString& PathFileName)
{
    for (UStaticMesh* Mesh : TObjectRange<UStaticMesh>())
    {
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
	for (UMaterial* Material : TObjectRange<UMaterial>())
	{
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
