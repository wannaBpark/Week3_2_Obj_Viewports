#pragma once
#include "Factory.h"
#include "Core/Container/Array.h"
#include "Object/Mesh/StaticMesh.h"

class FStaticMeshFactory : public FFactory
{
public:
	bool Import(const FString& FilePath) override;

private:
	bool ProcessObjFile(const FString& FilePath);
	bool ProcessMtlFile(const TArray<FObjMaterialInfo>& BuiltMaterials);
	void CopyTextureFile(const FString& TexturePaths);

	// FFactory을(를) 통해 상속됨
	bool SaveAsTAsset(const FString& SavePath) override;
};

