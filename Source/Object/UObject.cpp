#include "UObject.h"
#include "UClass.h"


UObject::UObject()
{
	Name = "None";
	ClassType = nullptr;
}

UObject::~UObject()
{
}

UClass* UObject::StaticClass()
{
	static std::unique_ptr<UClass, UClassDeleter> StaticClassInfo = nullptr;
	if (!StaticClassInfo)
	{
		constexpr size_t ClassSize = sizeof(UClass);
		void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(ClassSize);
		UClass* ClassPtr = reinterpret_cast<UClass*>(new(RawMemory) UClass("UObject", nullptr));
		StaticClassInfo = std::unique_ptr<UClass, UClassDeleter>(ClassPtr, UClassDeleter{});
	}
	return StaticClassInfo.get();
}

bool UObject::IsA(const UClass* OtherClass) const
{
	const UClass* ThisClass = GetClass();
	return ThisClass->IsChildOf(OtherClass);
}
