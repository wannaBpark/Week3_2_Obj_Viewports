#pragma once
#include <string>
#include "Core/Container/Array.h"
#include "JsonSaveHelper.h"

struct FVector4;

class UTextAtlasManager
{
public:
	UTextAtlasManager();

	static void SetAtlasManager(int InWidth, int InHeight, int InColumns, int InRows, int InMarginX, int InMarginY);

	static void SetAtlasSize(int InWidth, int InHeight);
	static void SetGridLayout(int InColumns, int InRows);
	static void SetMargin(int InMarginX, int InMarginY);



public:
	static FVector4 GetCharUV(char character, uint32 IndexNum);
	static FVector4 GetCharUV(char character, std::string AtlasName);
	static uint32 GetTextureIndex(std::string AtlasName);

private:
	static FVector4 GetCharUV(char character);

private:
	static inline int AtlasWidth{ 512 };
	static inline int AtlasHeight{ 512 };
	static inline int Columns{ 16 };
	static inline int Rows{ 16 };
	static inline int MarginX{ 0 };
	static inline int MarginY{ 0 };


public:
	static void AddAtlasInfo(UAtlasInfo& InAtlasInfo);

private:
	static inline TArray<UAtlasInfo> AtlasInfos;
};
