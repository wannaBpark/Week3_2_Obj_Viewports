#pragma once

#include <string>

#include "Core/HAL/PlatformType.h"
#include "Core/Math/Vector.h"

struct UObjectInfo
{
    FVector Location; 
    FVector Rotation;
    FVector Scale;
    std::string ObjectType;

    uint32 UUID;
};

struct UWorldInfo
{
    UObjectInfo** ObjctInfos;
    uint32 ActorCount;
    uint32 Version;
    std::string SceneName;
};

struct UAtlasInfo 
{
    std::string AtlasName;
    uint32 RowNum;
    uint32 ColNum;
    uint32 AtlasWidth;
    uint32 AtlasHeight;
    uint32 TextureIndex;
    uint32 TotalFrame;
};

class JsonSaveHelper
{
public:
    // SceneName - 확장자 제외
    static UWorldInfo* LoadScene(std::string SceneName);
    static void SaveScene(const UWorldInfo& WorldInfo);

    static UAtlasInfo LoadAtlasInfo(std::string AtlasName);
    static bool SaveAtlasInfo(const UAtlasInfo& AtlasInfo);
};
