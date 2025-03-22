#pragma once
#include "Core/Container/Array.h"
#include "Core/Container/String.h"
#include "Core/Container/Map.h"

#include <fstream>

struct FVector;
struct FVector2D;
struct FFaceInfo;
struct FObjMaterialInfo;

class FObjImporter
{
public:
    FObjImporter();
    
    FObjImporter(const FString& InFilePath);
    
    ~FObjImporter();

    FVector GetVertex(int32 Idx);
    
    FVector GetNormal(int32 Idx);
    
    FVector2D GetUV(int32 Idx);
    
    TArray<FFaceInfo> GetFaces(const FString& GroupName = TEXT("Default"))
    { 
		if (FacesPerGroup.Contains(GroupName))
		{
			return FacesPerGroup[GroupName];
		}
        return {};
    }

    TArray<uint32> GetVertexIndices();

    uint32 GetVertexNum() const { return Vertices.Num(); }

    uint32 GetNormalNum() const { return Normals.Num(); }

    uint32 GetUVNum() const { return UVs.Num(); }

    uint32 GetFaceNum(const FString& GroupName) const 
    {
		if (FacesPerGroup.Contains(GroupName))
		{
			return FacesPerGroup[GroupName].Num();
		}
        return 0;
    }

    TArray<FString> GetGroupNames() const
    {
        TArray<FString> GroupNames;
        for (auto& Kvp : FacesPerGroup)
        {
            GroupNames.Add(Kvp.first);
        };
        return GroupNames;
    }

    TMap<FString, FObjMaterialInfo> GetMaterials() const { return MaterialsPerGroup; }
	TArray<FVector> GetVertices() const { return Vertices; }

protected:
    FString FilePath;
    
    std::ifstream File;

    void SetFilePath(const FString& InFilePath);

    bool CheckFile(const FString& InFilePath) const;
    
    void ReadFile();

    void Clear();

    TArray<uint32> ParseFaceInfoPoint(const std::string& line);

    bool LoadMTL(const FString& filename);

    FString MaterialName = "";
    TArray<FVector> Vertices;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TMap<FString, TArray<FFaceInfo>> FacesPerGroup;
	TMap<FString, FObjMaterialInfo> MaterialsPerGroup;
};
