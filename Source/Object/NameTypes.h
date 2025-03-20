#pragma once

#include "Core/HAL/PlatformType.h"

class FString;

struct FName
{
    friend struct FNameHelper;

    int32 DisplayIndex;
    int32 ComparisonIndex;
public:

	FName() : DisplayIndex(0), ComparisonIndex(0) {}
    FName(const ANSICHAR* Name);
    FName(const WIDECHAR* Name);
    FName(const FString& Name);

    
    FString ToString() const;
	uint32 GetDisplayIndex() const const { DisplayIndex; }
	uint32 GetComparisonIndex() const {	ComparisonIndex; }

    FORCEINLINE bool operator==(const FName& Other) const;

};