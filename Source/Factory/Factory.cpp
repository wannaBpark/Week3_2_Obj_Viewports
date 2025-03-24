#include "Factory.h"
#include <filesystem>
#include <Debug/DebugConsole.h>

namespace fs = std::filesystem;

std::unique_ptr<FFactory> FFactory::CreateFactory(const FString& FactoryName)
{
	FString Extension = GetFileExtension(FactoryName);

	auto& FactoryMap = GetFactoryMap();

	auto it = FactoryMap.Find(Extension);

	if (it != nullptr)
	{
		return (*it)();
	}

	UE_LOG("Extension %s is not supported", *Extension);
	return nullptr;
}

FString FFactory::GetFileExtension(const FString& FilePath)
{
	fs::path Path = FilePath.c_char();
	std::string Extension = Path.extension().string();
	std::transform(Extension.begin(), Extension.end(), Extension.begin(), ::tolower);
	return FString(Extension.c_str());
}

FString FFactory::CreateOutputFilePath(const FString& OriginalPath, const FString& SubFolder)
{
	fs::path Path(OriginalPath.c_char());
	fs::path OutputDir = fs::path("Assets") / SubFolder.c_char();
	fs::path OutputPath = OutputDir / Path.stem().concat(".tasset");

	return FString(OutputPath.string());
}

bool FFactory::SaveAsTAsset()
{
	// !TODO : FArchive에서 구현해야 함
	try 
	{
		return true;
	}
	catch (const std::exception& e)
	{
		return false;
	}
}
