#pragma once
#include <string>
#include "Object/UObject.h"
#include "Core/Container/Map.h"


class UClass : public UObject
{ 
public:
	UClass() { }
	UClass(const char* InName, UClass* InParentClass);
	~UClass();

	UClass* GetParentClass() const { return ParentClass; }

private:
	UClass* ParentClass;

public:
	bool IsChildOf(const UClass* OtherClass) const;

	template <typename T>
		requires std::derived_from<T, UObject>
	bool IsChildOf() const {
		return IsA(T::StaticClass());
	}
};

struct UClassDeleter
{
	void operator()(UClass* ClassPtr) const
	{
		ClassPtr->~UClass();
		FPlatformMemory::DecrementObjectStats(sizeof(UClass));
		StackAllocator::GetInstance().deleteNode(ClassPtr);
	}
};