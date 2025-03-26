#pragma once
#include "Core/Container/Map.h"
#include "Object/Mesh/StaticMesh.h"
#include "Core/Container/String.h"
#include "Object/NameTypes.h"


// 메시 및 머터리얼 관리 Static 클래스
// !Note
// FStaticMesh의 생성 시점 -> 프리로드를 하면 생성된다 -> FStaticMesh 프리로드 시점에 MaterialMap도 구성된다
// UStaticMesh의 생성 시점 -> LoadObjStaticMesh 시점에 생성
// UMaterial의 생성 시점 -> LoadMaterial 시점에 생성
class FObjManager
{
public:
	// obj로부터 import
	static FStaticMesh* ImportObjStaticMeshAsset(const FString& PathFileName, TArray<FObjMaterialInfo>* OutMaterials = nullptr);
	// tsmesh로부터 Load
	static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);
	static class UStaticMesh* LoadObjStaticMesh(const FString& PathFileName);

	static class UMaterial* LoadMaterial(const FName& MaterialName);
	static void ReleaseResources();

	static TMap<FString, FStaticMesh*>& GetObjStaticMeshMap() { return ObjStaticMeshMap; }
	static TMap<FName, FObjMaterialInfo>& GetMaterialMap() { return MaterialMap; }
private:
	static TMap<FString, FStaticMesh*> ObjStaticMeshMap;
	static TMap<FName, FObjMaterialInfo> MaterialMap;
	static FString GetNameFromPath(const FString& FilePath);
};
