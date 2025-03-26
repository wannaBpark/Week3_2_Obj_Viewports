#pragma once
#include "Core/Container/String.h"
#include "Core/Math/Transform.h"
#include "Core/Container/Array.h"
#include "Serialization/Archive.h"

struct FActorComponentInfo
{
	FString Name;
	FTransform Transform;
	TArray<FString> Args;

	void Serialize(FArchive& ar) const
	{
		ar << Name;
		ar << Transform;
		ar << Args;
	}

	void Deserialize(FArchive& ar)
	{
		ar >> Name;
		ar >> Transform;
		ar >> Args;
	}
};