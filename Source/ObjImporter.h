#pragma once
#include "Core/Container/Array.h"
#include "Core/Container/String.h"
#include "Core/Container/Map.h"
#include "Object/NameTypes.h"

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
    
    TArray<FFaceInfo> GetFaces(const FName& GroupName = TEXT("Default"))
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

    TArray<FName> GetGroupNames() const
    {
        TArray<FName> GroupNames;
        for (auto& Kvp : FacesPerGroup)
        {
            GroupNames.Add(Kvp.first);
        };
        return GroupNames;
    }

    TMap<FName, FObjMaterialInfo> GetMaterials() const { return MaterialsMap; }
	TArray<FVector> GetVertices() const { return Vertices; }
	TMap<FName, FName> GetMaterialPerGroup() const { return MaterialPerGroup; }

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
	// key는 그룹이름, value는 Face정보
    TMap<FName, TArray<FFaceInfo>> FacesPerGroup;
	// key = 이름, value = 머티리얼 정보
	TMap<FName, FObjMaterialInfo> MaterialsMap;
	// key = 그룹이름, value = 머티리얼 이름
	TMap<FName, FName> MaterialPerGroup;
};
