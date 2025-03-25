#pragma once
#include "Serialization/Archive.h"
#include "Core/Container/String.h"
#include "Core/Math/Transform.h"
#include "Core/Container/Array.h"
#include "Object/ActorComponent/ActorComponentInfo.h"

// Actor의 Deserialize 시점에 필요한 MetaData 구조체
struct FActorInfo
{
	FString Type;
	FTransform ActorTransform;
	TArray<FActorComponentInfo> ComponentInfos;

	void Serialize(FArchive& Ar) const
	{
		Ar << Type;
		Ar << ActorTransform;
		Ar << ComponentInfos;
	}

	void Deserialize(FArchive& Ar)
	{
		Ar >> Type;
		Ar >> ActorTransform;
		Ar >> ComponentInfos;
	}
};