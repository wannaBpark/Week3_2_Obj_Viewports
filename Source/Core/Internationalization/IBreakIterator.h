#pragma once
#include <Core/Container/String.h>

class IBreakIterator
{
public:
	virtual ~IBreakIterator() {}

	virtual void SetString(const TCHAR* const InString, const int32 InStringLength)
	{
		SetString(FString(InString, InStringLength));
	}

	virtual void SetString(const FString& InString) = 0;

	virtual void SetStringRef(const FString* InString) = 0;
};