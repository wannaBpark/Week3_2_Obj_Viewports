#pragma once
#include "Core/Container/Map.h"
#include "Object/Mesh/StaticMesh.h"
#include "Core/Container/String.h"
#include "Object/NameTypes.h"


// 메시 및 머터리얼 관리 Static 클래스
// !TODO : FStaticMesh 해제 로직
class FObjManager
{
public:
	static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);
	static class UStaticMesh* LoadObjStaticMesh(const FString& PathFileName);

	static class UMaterial* LoadMaterial(const FName& MaterialName);
private:
	static TMap<FName, FStaticMesh*> ObjStaticMeshMap;
	static TMap<FName, FObjMaterialInfo> MaterialMap;
};

