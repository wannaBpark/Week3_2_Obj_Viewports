#pragma once
#include <memory>
#include "Core/Container/String.h"
#include "Core/Container/Map.h"
#include <functional>
#include "Assets/AssetDefine.h"
#include "Object/NameTypes.h"

struct FFactoryRegister;

#define REGISTER_FACTORY(ClassName, Extension)\
static FFactory::FFactoryRegister AutoRegister##ClassName(Extension, []{\
	return std::make_unique<ClassName>();\
});

class FFactory
{
public:
	virtual ~FFactory() = default; 

public:
	// 자동 등록 매크로용 구조체
	struct FFactoryRegister
	{
		FFactoryRegister(const FString& Extension, std::function<std::unique_ptr<FFactory>()> FactoryFunc)
		{
			GetFactoryMap()[Extension] = FactoryFunc;
		}
	};

	template<typename T>
	static void RegisterFactory(const FString& Extenstion)
	{
		auto Factory = []() -> std::unique_ptr<FFactory>
			{
				return std::make_unique<T>();
			};

		GetFactoryMap()[Extenstion] = Factory;
	}


	static std::unique_ptr<FFactory> CreateFactory(const FString& FactoryName);

	// 각 Factory들이 구현해야 할 자신의 Import로직
	virtual bool Import(const FString& FilePath) = 0;

public:
	// 외부에서 간단히 호출할 수 있도록 한 static 함수
	static bool ImportFile(const FString& FilePath);


protected:
	// 공용 유틸리티 함수
	static FString GetFileExtension(const FString& FilePath);
	static FString CreateOutputFilePath(const FString& OriginalPath, const FString& SubFolder);
	static FString CreateHeader(const FString& OriginalFIlePath, const FString& FileType);
	static bool ParseHeader(const FString& FilePath, FString& OutFileType, FString& OutSourceFile);

private:
	static TMap<FString, std::function<std::unique_ptr<FFactory>()>>& GetFactoryMap()
	{
		static TMap<FString, std::function<std::unique_ptr<FFactory>()>> FactoryMap;
		return FactoryMap;
	}
};

class FLoadEvent
{
public:
	virtual ~FLoadEvent() = default;
};

