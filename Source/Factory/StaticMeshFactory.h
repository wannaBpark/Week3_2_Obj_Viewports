#pragma once
#include "Factory.h"
#include "Core/Container/Array.h"
#include "Object/Mesh/StaticMesh.h"
#include <functional>

class FStaticMeshFactory : public FFactory
{
public:
	bool Import(const FString& FilePath) override;

private:
	bool ProcessObjFile(const FString& FilePath);
	bool ProcessMtlFile(TArray<FObjMaterialInfo>& BuiltMaterials);
	FString CopyTextureFile(const FString& TexturePaths);

	void SaveStaticMeshAsset(const FString& FilePath, FStaticMesh& StaticMesh);
	void SaveMaterialAsset(const FObjMaterialInfo& BuiltMaterials);
};
