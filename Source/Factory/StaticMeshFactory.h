#pragma once
#include "Factory.h"
#include "Core/Container/Array.h"

class FStaticMeshFactory : public FFactory
{
public:
	bool Import(const FString& FilePath) override;

private:
	bool ProcessObjFile(const FString& FilePath);
	bool ProcessMtlFile(const FString& FilePath);
	void CopyTextureFiles(const TArray<FString>& TexturePaths);
};

