#include "Factory.h"
#include <filesystem>
#include <sstream>
#include <fstream>
#include <Debug/DebugConsole.h>

namespace fs = std::filesystem;

std::unique_ptr<FFactory> FFactory::CreateFactory(const FString& InExtenstion)
{
	FString Extension = GetFileExtension(InExtenstion);

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

FString FFactory::CreateHeader(const FString& OriginalFIlePath, const FString& FileType)
{
	FString Header;

	Header += TEXT("TASSET FILE\n");
	Header += TEXT("FileType: ") + FileType + TEXT("\n");
	Header += TEXT("SourceFile: ") + OriginalFIlePath + TEXT("\n");
	Header += TEXT("Header End\n");

	return Header;
}

bool FFactory::ParseHeader(const FString& FilePath, FString& OutFileType, FString& OutSourceFile)
{
	if (!fs::exists(FilePath.c_char()))
	{
		UE_LOG("File %s is not exist", *FilePath);
		return false;
	}

	std::ifstream File(FilePath.c_char());

	bool HasFileType = false;
	bool HasSourceFile = false;
	bool HasHeaderEnd = false;


	std::string Line;
	while (std::getline(File, Line))
	{
		if (Line.starts_with("FileType: "))
		{
			OutFileType = Line.substr(10);
			HasFileType = true;
		}
		else if (Line.starts_with("SourceFile: "))
		{
			OutSourceFile = Line.substr(12);
			HasSourceFile = true;
		}
		else if (Line.starts_with("Header End"))
		{
			HasHeaderEnd = true;
			break;
		}
	}

	return HasFileType && HasSourceFile && HasHeaderEnd;
}
