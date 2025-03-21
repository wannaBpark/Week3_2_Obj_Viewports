#pragma once
#include "Core/Container/Map.h"
#include "Core/Container/String.h"
#include "Object/Mesh/StaticMesh.h"

class FObjManager
{
private:
	static TMap<FString, FStaticMesh*> ObjStaticMeshMap;

public:
	static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);

};

