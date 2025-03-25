#pragma once
#include <memory>
#include "Core/Container/String.h"
#include "Core/Container/Map.h"
#include <functional>

class FFactory
{
public:
	virtual ~FFactory() = default; 

	static std::unique_ptr<FFactory> CreateFactory(const FString& FactoryName);

	template<typename T>
	static void RegisterFactory(const FString& Extenstion)
	{
		auto Factory = []() -> std::unique_ptr<FFactory>
			{
				return std::make_unique<T>();
			};

		GetFactoryMap()[Extenstion] = Factory;

	}

	virtual bool Import(const FString& FilePath) = 0;

protected:
	// 공용 유틸리티 함수
	static FString GetFileExtension(const FString& FilePath);
	static FString CreateOutputFilePath(const FString& OriginalPath, const FString& SubFolder);
	static FString CreateHeader(const FString& OriginalFIlePath, const FString& FileType);
	static bool ParseHeader(const FString& FilePath, FString& OutFileType, FString& OutSourceFile);

	virtual bool SaveAsTAsset(const FString& SavePath) = 0; // 

private:
	static TMap<FString, std::function<std::unique_ptr<FFactory>()>>& GetFactoryMap()
	{
		static TMap<FString, std::function<std::unique_ptr<FFactory>()>> FactoryMap;
		return FactoryMap;
	}
};

