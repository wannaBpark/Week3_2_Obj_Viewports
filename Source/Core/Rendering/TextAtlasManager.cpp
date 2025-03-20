#include "TextAtlasManager.h"
#include "Debug/DebugConsole.h"
#include "Core/Math/Vector.h"


UTextAtlasManager::UTextAtlasManager()
{

}

void UTextAtlasManager::SetAtlasManager(int InWidth, int InHeight, int InColumns, int InRows, int InMarginX, int InMarginY)
{
	SetAtlasSize(InWidth, InHeight);
	SetGridLayout(InColumns, InRows);
	SetMargin(InMarginX, InMarginY);
}

void UTextAtlasManager::SetAtlasSize(int InWidth, int InHeight)
{
	AtlasWidth = InWidth;
	AtlasHeight = InHeight;
}

void UTextAtlasManager::SetGridLayout(int InColumns, int InRows)
{
	Columns = InColumns;
	Rows = InRows;
}

void UTextAtlasManager::SetMargin(int InMarginX, int InMarginY)
{
	MarginX = InMarginX;
	MarginY = InMarginY;
}

FVector4 UTextAtlasManager::GetCharUV(char character, uint32 IndexNum)
{
    if (AtlasInfos.Num() < IndexNum) 
    {
        UE_LOG("UTextAtlasManager Invalid IndexNum!");
        return FVector4();
    }

    UAtlasInfo AtlasInfo = AtlasInfos[IndexNum];
    SetAtlasManager(AtlasInfo.AtlasWidth, AtlasInfo.AtlasHeight, AtlasInfo.ColNum, AtlasInfo.RowNum, 0, 0);
    return GetCharUV(character);
}

FVector4 UTextAtlasManager::GetCharUV(char character, std::string AtlasName)
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
        UE_LOG("UTextAtlasManager Invalid AtlasName!");
        return FVector4();
    }

    SetAtlasManager(AtlasInfo.AtlasWidth, AtlasInfo.AtlasHeight, AtlasInfo.ColNum, AtlasInfo.RowNum, 0, 0);
    return GetCharUV(character);
}

uint32 UTextAtlasManager::GetTextureIndex(std::string AtlasName)
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

FVector4 UTextAtlasManager::GetCharUV(char character)
{
    FVector4 rect = { 0.f, 0.f, 0.f, 0.f };
    if (character > Columns * Rows - 1 || Columns <= 0 || Rows <= 0 || AtlasWidth <= 0 || AtlasHeight <= 0) {
        UE_LOG("TextAtlasManager GetCharUV Error!");
        return rect; // 잘못된 입력이면 기본값 반환
    }

    int index = character; //  현재 준비된 TextAtlas는 char을 바로 사용 가능

    // 효과적으로 사용 가능한 영역에서 각 셀의 크기 계산
    int availableWidth = AtlasWidth - (Columns - 1) * MarginX;
    int availableHeight = AtlasHeight - (Rows - 1) * MarginY;
    int cellWidth = availableWidth / Columns;
    int cellHeight = availableHeight / Rows;

    int col = index % Columns;
    int row = index / Columns;
    if (row >= Rows) {
        UE_LOG("TextAtlasManager GetCharUV Error row!");
        return rect;
    }

    // 각 셀의 시작 좌표 (픽셀 단위)
    int x = col * (cellWidth + MarginX);
    int y = row * (cellHeight + MarginY);

    // 정규화된 UV 좌표 계산 (0.0 ~ 1.0)
    rect.X = static_cast<float>(cellWidth) /  static_cast<float>(AtlasWidth);
    rect.Y = static_cast<float>(cellHeight) / static_cast<float>(AtlasHeight);
    rect.Z = static_cast<float>(x) / static_cast<float>(AtlasWidth);
    rect.W = static_cast<float>(y) / static_cast<float>(AtlasHeight);

    return rect;
}

void UTextAtlasManager::AddAtlasInfo(UAtlasInfo& InAtlasInfo)
{
    UAtlasInfo AtlasInfo = UAtlasInfo(InAtlasInfo);
    
    AtlasInfos.Add(AtlasInfo);
}
