#pragma once
#define DEFAULT_INI_FILE_PATH "\\engine.ini"

#define SECTION_MAPPING(Section, Key) { EEngineConfigSectionType::Section, TEXT(Key) }
#define CONFIG_MAPPING(Config, Key, SectionType) { EEngineConfigValueType::Config, TEXT(Key), EEngineConfigSectionType::SectionType }
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Core/Container/Map.h"
#include "ThirdParty/iniparser.hpp"
#include "Template/Template.h"

// engine.ini의 Section 타입 
enum class EEngineConfigSectionType
{
	ECS_None,

	ECS_Screen,
	ECS_Camera,
	ECS_Max,
};

// engine.ini의 Value 타입
enum class EEngineConfigValueType
{
	EEC_None,
	// 화면
	EEC_ScreenWidth,
	EEC_ScreenHeight,

	// 에디터
	EEC_EditorCameraPosX,
	EEC_EditorCameraPosY,
	EEC_EditorCameraPosZ,

	EEC_EditorCameraRotX,
	EEC_EditorCameraRotY,
	EEC_EditorCameraRotZ,
	EEC_EditorCameraRotW,

	EEC_EditorCameraSpeed,
	EEC_EditorCameraSensitivity,

	EEC_Max,
};

struct SectionMapping
{
	EEngineConfigSectionType Section;
	const FString Key;
};

struct ConfigMapping
{
	EEngineConfigValueType Config;
	const FString Key;
	EEngineConfigSectionType Section;
};

// ini에 정의될 Section 매핑
static const SectionMapping SectionMappings[] =
{
	SECTION_MAPPING(ECS_None, "NONE"),
	SECTION_MAPPING(ECS_Screen, "Screen"),
	SECTION_MAPPING(ECS_Camera, "Camera")
	// !TODO : EngineConfigSection 추가시 추가
};

// ini에 정의될 Config들을 매핑
static const ConfigMapping ConfigMappings[] = {
	CONFIG_MAPPING(EEC_None, "None", ECS_None),
	CONFIG_MAPPING(EEC_ScreenWidth, "ScreenWidth", ECS_Screen),
	CONFIG_MAPPING(EEC_ScreenHeight, "ScreenHeight", ECS_Screen),

	CONFIG_MAPPING(EEC_EditorCameraPosX, "EditorCameraPosX", ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraPosY, "EditorCameraPosY", ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraPosZ, "EditorCameraPosZ", ECS_Camera),

	CONFIG_MAPPING(EEC_EditorCameraRotX, "EditorCameraRotX", ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraRotY, "EditorCameraRotY", ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraRotZ, "EditorCameraRotZ", ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraRotW, "EditorCameraRotW", ECS_Camera),

	CONFIG_MAPPING(EEC_EditorCameraSpeed, "EditorCameraSpeed", ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraSensitivity, "EditorCameraSensitivity", ECS_Camera)

	
	// !TODO : EngineConfig 추가시 추가
};

class UEngine;

class FEngineConfig : public FNoncopyable
{
	friend class UEngine;

private:
	FEngineConfig();
	~FEngineConfig();

public:
	void LoadEngineConfig();
	template<typename T>
	void SaveEngineConfig(EEngineConfigValueType InConfig, T InValue)
	{
		auto Section = FindSection(InConfig);
		if (Section == EEngineConfigSectionType::ECS_None)
		{
			return;
		}

		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>)
		{
			EngineConfig[Section][InConfig] = FString::SanitizeFloat(InValue);
		}
		else if constexpr(std::is_same_v<T, FString>)
		{
			EngineConfig[Section][InConfig] = InValue;
		}
		else
		{
			static_assert(!sizeof(T*), "Unsupported type for SaveEngineConfig");
		}
		
		std::string SectionStr(SectionMappings[static_cast<int>(Section)].Key.c_char());
		auto Section_ft = ft.GetSection(SectionStr);

		if (Section_ft)
		{
			Section_ft->SetValue(std::string(ConfigMappings[static_cast<int>(InConfig)].Key.c_char()), InValue);
		}

		ft.Save(std::string(Path.c_char()));
	}

	template<typename T>
	T GetEngineConfigValue(EEngineConfigValueType InConfig, T DefaultValue = T()) const
	{
		auto Section = FindSection(InConfig);
		if (Section == EEngineConfigSectionType::ECS_None)
		{
			return DefaultValue;
		}

		if(!EngineConfig.Contains(Section))
		{
			return DefaultValue;
		}

		if (!EngineConfig[Section].Contains(InConfig))
		{
			return DefaultValue;
		}

		const FString& ValueStr = EngineConfig[Section][InConfig];

		try
		{
			if constexpr (std::is_same_v<T, int>)
			{
				return std::stoi(*ValueStr);
			}
			else if constexpr (std::is_same_v<T, float>)
			{
				return std::stof(*ValueStr);
			}
			else if constexpr (std::is_same_v<T, FString>)
			{
				return *ValueStr;
			}
			else
			{
				static_assert(!sizeof(T*), "Unsupported type for GetEngineConfigValue");
				return DefaultValue;
			}
		}
		catch (const std::invalid_argument& e)
		{
			return DefaultValue;
		}
		catch (const std::out_of_range& e)
		{
			return DefaultValue;
		}
	}
	void SaveAllConfig();

private:
	EEngineConfigSectionType FindSection(const EEngineConfigValueType& InValueType) const;
	TArray<ConfigMapping> GetConfigList(const EEngineConfigSectionType& InSectionType) const;
	FString GetPath();
private:
	TMap<EEngineConfigSectionType, TMap<EEngineConfigValueType, FString>> EngineConfig;

	INI::File ft;
	FString Path;
};