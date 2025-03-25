#pragma once
#include "Archive.h"
class ISerializable
{
	virtual ~ISerializable() = default;

	virtual void Serialize(FArchive& Archive) const = 0;
	virtual void Deserialize(FArchive& Archive) = 0;

	friend FArchive& operator<<(FArchive& Archive, const ISerializable& Object)
	{
		Object.Serialize(Archive);
		return Archive;
	}

	friend FArchive& operator>>(FArchive& Archive, ISerializable& Object)
	{
		Object.Deserialize(Archive);
		return Archive;
	}
};

