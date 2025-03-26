#include "ObjManager.h"
#include "UStaticMesh.h"
#include "Core/Container/ObjectIterator.h"
#include "Core/Engine.h"
#include "Object/ObjectFactory.h"
#include "Object/Material/Material.h"
#include <filesystem>
#include "MeshBuilder.h"
#include "Serialization/FWindowsBinHelper.h"
#include <Assets/AssetDefine.h>

TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;
TMap<FName, FObjMaterialInfo> FObjManager::MaterialMap;

FStaticMesh* FObjManager::ImportObjStaticMeshAsset(const FString& PathFileName, TArray<FObjMaterialInfo>* OutMaterials)
{
	FString MeshNameStr = GetNameFromPath(PathFileName);
    FStaticMesh** MeshDataPtr = ObjStaticMeshMap.Find(MeshNameStr);

	if (MeshDataPtr)
		return *MeshDataPtr;

    FMeshBuilder MeshBuilder;
    MeshBuilder.BuildMeshFromObj(PathFileName);

	FStaticMesh* MeshData = new FStaticMesh();
	MeshData->Name = MeshNameStr;
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

	ObjStaticMeshMap.Add(GetNameFromPath(PathFileName), MeshData);

    return MeshData;
}

FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName)
{
	FString MeshNameStr = GetNameFromPath(PathFileName);
	FStaticMesh** MeshDataPtr = ObjStaticMeshMap.Find(MeshNameStr);

	if (MeshDataPtr)
		return *MeshDataPtr;

	FStaticMesh* StaticMesh = new FStaticMesh();

	FArchive StaticMeshAr;
	FWindowsBinHelper::LoadFromBin(PathFileName, StaticMeshAr);
	if (StaticMeshAr.IsEmpty())
	{
		UE_LOG("%s Asset is Not Imported!!!!", *MeshNameStr);
		return nullptr;
	}
	else
	{
		StaticMeshAr >> *StaticMesh;
		ObjStaticMeshMap.Add(StaticMesh->Name, StaticMesh);
	}


	// Load Materials
	for (auto& Kvp : StaticMesh->SubMeshes)
	{
		FArchive MaterialAr;
		FString MaterialName = Kvp.second.MaterialName.ToString();
		FString MaterialPath = ASSET_DEFAULT_MATERIAL_PATH + MaterialName + ASSET_DEFAULT_MATERIAL_EXTENSION;
		FWindowsBinHelper::LoadFromBin(MaterialPath, StaticMeshAr);
		if (!StaticMeshAr.IsEmpty())
		{
			FObjMaterialInfo MaterialInfo;
			StaticMeshAr >> MaterialInfo;
			
			// 읽어온 텍스쳐를 SRV로 만들어서 인덱스를 저장
			int idx = UEngine::Get().GetRenderer()->CreateTextureSRVW(MaterialInfo.TexturePath.c_wchar());
			MaterialInfo.TextureMapIndex = idx;

			MaterialMap.Add(Kvp.second.MaterialName, MaterialInfo);
		}
	}
	return StaticMesh;
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
	if (MeshAsset == nullptr)
	{
		return nullptr;
	}
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

FString FObjManager::GetNameFromPath(const FString& FilePath)
{
	std::filesystem::path path(FilePath.c_char());
	return FString(path.stem().string().c_str()); 
}
