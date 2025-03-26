#pragma once
#include <filesystem>
#include "Core/Container/String.h"

namespace fs = std::filesystem;
class FPath
{
public:
	static FString GetPureFilename(const FString& FilePath)
	{
		fs::path Path = FilePath.c_char();
		return Path.filename().string();
	}

	static FString GetRelativeFilePath(const FString& FilePath)
	{
		fs::path Path = FilePath.c_char();
		return Path.relative_path().string();
	}
};