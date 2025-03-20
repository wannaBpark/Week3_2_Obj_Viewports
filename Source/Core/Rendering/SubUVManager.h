#pragma once
#include <vector>
#include "Core/Container/Array.h"
#include "JsonSaveHelper.h"

struct FVector4;

class USubUVManager
{
public:
    USubUVManager();

    static void Initialize(int InWidth, int InHeight, int InColumns, int InRows, int TotalFrames);
    static void SetTextureSize(int InWidth, int InHeight);
    static void SetGridLayout(int InColumns, int InRows);
    static void SetTotalFrames(int InTotalFrames);
    static int GetTotalFrames() {
        return TotalFrames;
    }

public:
    static FVector4 GetFrameUV(int FrameIndex, uint32 IndexNum);
    static FVector4 GetFrameUV(int FrameIndex, std::string AtlasName);
    static uint32 GetTextureIndex(std::string AtlasName);

private:
    static FVector4 GetFrameUV(int FrameIndex);

private:
    static inline int AtlasWidth{ 2000 };
    static inline int AtlasHeight{ 2025 };
    static inline int Columns{ 5 };
    static inline int Rows{ 9 };
    static inline int TotalFrames{ 42 }; 

    static inline float CellWidth{400};
    static inline float CellHeight{225};

    static void CalculateCellSize();

public:
    static void AddAtlasInfo(UAtlasInfo& InAtlasInfo);

private:
    static inline TArray<UAtlasInfo> AtlasInfos;
};
