#pragma once

#include "Core/HAL/PlatformType.h"
#include "Core/Container/String.h"
#include <xhash>

class FString;

struct FName
{
    friend struct FNameHelper;

    int32 DisplayIndex;
    int32 ComparisonIndex;
public:

	static bool Equals(FName A, FName B) = delete;
	
	FName() : DisplayIndex(0), ComparisonIndex(0) {}
    FName(const ANSICHAR* Name);
    FName(const WIDECHAR* Name);
    FName(const FString& Name);

    
    FString ToString() const;
	uint32 GetDisplayIndex() const { DisplayIndex; }
	uint32 GetComparisonIndex() const {	ComparisonIndex; }

    FORCEINLINE bool operator==(const FName& Other) const
    {
		return ComparisonIndex == Other.ComparisonIndex;
    }

	FORCEINLINE bool operator==(const FName& Other) const;
};

// FName의 해시 함수
template <>
struct std::hash<FName>
{
	size_t operator()(const FName& InName) const
	{
		// 해시 계산 로직
		return std::hash<FString::BaseStringType>()(*InName.ToString());
	}
};