#pragma once
#include "Core/Container/Map.h"
#include "Object/Mesh/StaticMesh.h"
#include "Core/Container/String.h"
#include "Object/NameTypes.h"


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

