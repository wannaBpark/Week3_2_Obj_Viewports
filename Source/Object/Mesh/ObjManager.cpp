#include "ObjManager.h"
#include "UStaticMesh.h"
#include "Core/Container/ObjectIterator.h"
#include "Core/Engine.h"
#include "Object/ObjectFactory.h"

TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;

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
	MeshData->Materials = MeshBuilder.GetMaterials();


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
