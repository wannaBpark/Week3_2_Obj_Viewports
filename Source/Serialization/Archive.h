#pragma once
#include "ThirdParty/cereal/cereal.hpp"
#include "ThirdParty/cereal/archives/binary.hpp"
#include "Core/Container/String.h"
#include <sstream>
#include <fstream>

class FArchive
{
private:
	std::stringstream Stream;

public:
	template<typename T>
	void Save(T& Value)
	{
		cereal::BinaryOutputArchive OutArchive(Stream);
		OutArchive(Value);
	}

	template<typename T>
	void Load(T& Value)
	{
		cereal::BinaryInputArchive InArchive(Stream);
		InArchive(Value);
	}

	std::string GetSerializedData() const 
	{
		return Stream.str();
	}

	void SetSeralizedData(const std::string& Data)
	{
		Stream.str(Data);
		Stream.clear();
	}

	template<typename T>
	friend FArchive& operator<<(FArchive& Archive, T& Value)
	{
		Archive.Save(Value);
		return Archive;
	}

	template<typename T>
	friend FArchive& operator>>(FArchive& Archive, T& Value)
	{
		Archive.Load(Value);
		return Archive;
	}

	bool SaveToFile(const FString& FileName)
	{
		std::ofstream OutFile(FileName.c_char(), std::ios::binary);
		if (!OutFile.is_open())
		{
			return false;
		}

		std::string Data = GetSerializedData();
		OutFile.write(Data.c_str(), Data.size());
		OutFile.close();

		return true;
	}

	bool LoadFromFile(const FString& FileName)
	{
		std::ifstream InFile(FileName.c_char(), std::ios::binary);
		if (!InFile.is_open())
		{
			return false;
		}
		std::stringstream Buffer;
		Buffer << InFile.rdbuf();
		SetSeralizedData(Buffer.str());
		InFile.close();
		return true;
	}
};

