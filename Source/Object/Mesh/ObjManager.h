#pragma once
#include "Core/Container/Map.h"

#include "Object/Mesh/StaticMesh.h"
#include "Core/Container/String.h"

class FObjManager
{
public:
	static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);
	static class UStaticMesh* LoadObjStaticMesh(const FString& PathFileName);

private:
	static TMap<FString, FStaticMesh*> ObjStaticMeshMap;
};

