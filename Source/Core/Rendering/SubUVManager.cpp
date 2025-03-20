#include "SubUVManager.h"
#include "Core/Math/Vector.h"
#include "Debug/DebugConsole.h"

USubUVManager::USubUVManager()
{
}

void USubUVManager::Initialize(int InWidth, int InHeight, int InColumns, int InRows, int InTotalFrames)
{
    SetTextureSize(InWidth, InHeight);
    SetGridLayout(InColumns, InRows);
    SetTotalFrames(InTotalFrames);
}

void USubUVManager::SetTextureSize(int InWidth, int InHeight)
{
    AtlasWidth = InWidth;
    AtlasHeight = InHeight;
    CalculateCellSize();
}

void USubUVManager::SetGridLayout(int InColumns, int InRows)
{
    Columns = InColumns;
    Rows = InRows;
    CalculateCellSize();
}

void USubUVManager::SetTotalFrames(int InTotalFrames)
{
    TotalFrames = InTotalFrames;
}

void USubUVManager::CalculateCellSize()
{
    if (Columns > 0 && Rows > 0)
    {
        CellWidth = 1.0f / Columns;
        CellHeight = 1.0f / Rows;
    }
}

void USubUVManager::AddAtlasInfo(UAtlasInfo& InAtlasInfo)
{
    UAtlasInfo AtlasInfo = UAtlasInfo(InAtlasInfo);

    AtlasInfos.Add(AtlasInfo);
}

FVector4 USubUVManager::GetFrameUV(int FrameIndex, uint32 IndexNum)
{
    if (AtlasInfos.Num() < IndexNum) 
    {
        UE_LOG("SubUVManager Invalid IndexNum!");
        return FVector4();
    }

    UAtlasInfo AtlasInfo = AtlasInfos[IndexNum];
    Initialize(AtlasInfo.AtlasWidth, AtlasInfo.AtlasHeight, AtlasInfo.ColNum, AtlasInfo.RowNum, AtlasInfo.TotalFrame);
    return GetFrameUV(FrameIndex);
}

FVector4 USubUVManager::GetFrameUV(int FrameIndex, std::string AtlasName)
{
    UAtlasInfo AtlasInfo;
    for (int i = 0; i < AtlasInfos.Num(); i++) 
    {
        if (AtlasInfos[i].AtlasName == AtlasName) 
        {
            AtlasInfo = UAtlasInfo(AtlasInfos[i]);
            break;
        }
    }

    if (AtlasInfo.AtlasName == "") 
    {
        UE_LOG("SubUVManager InValid AtlasName!");
        return FVector4();
    }

    Initialize(AtlasInfo.AtlasWidth, AtlasInfo.AtlasHeight, AtlasInfo.ColNum, AtlasInfo.RowNum, AtlasInfo.TotalFrame);
    return GetFrameUV(FrameIndex);
}

uint32 USubUVManager::GetTextureIndex(std::string AtlasName)
{
    UAtlasInfo AtlasInfo;
    for (int i = 0; i < AtlasInfos.Num(); i++) {
        if (AtlasInfos[i].AtlasName == AtlasName) {
            AtlasInfo = UAtlasInfo(AtlasInfos[i]);
            break;
        }
    }

    if (AtlasInfo.AtlasName == "")
    {
        UE_LOG("UTextAtlasManager Invalid AtlasName!");
        return -1;
    }

    return AtlasInfo.TextureIndex;
}

FVector4 USubUVManager::GetFrameUV(int FrameIndex)
{
    FVector4 UVRect = { 0.f, 0.f, 0.f, 0.f };

    /*if (FrameIndex >= TotalFrames || FrameIndex < 0 || Columns <= 0 || Rows <= 0)
    {
        UE_LOG("SubUVManager GetFrameUV Error!");
        return UVRect;
    }
    */

    int col = (FrameIndex % TotalFrames) % Columns ;
    int row = (FrameIndex % TotalFrames)/Columns;

    if (row >= Rows)
    {
        UE_LOG("SubUVManager GetFrameUV Error: Row out of range!");
        return UVRect;
    }

    // UV 계산
    UVRect.X = static_cast<float>(CellWidth);
    UVRect.Y = static_cast<float>(CellHeight);
    UVRect.Z = col * static_cast<float>(CellWidth);
    UVRect.W = row * static_cast<float>(CellHeight);

    return UVRect;
}
