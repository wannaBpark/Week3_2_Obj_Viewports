#pragma once
#include <memory>
#include "UClass.h"
#include "Object/ObjectFactory.h"

#define DECLARE_CLASS(TClass, TSuperClass) \
public: \
	using Super = TSuperClass; \
	using ThisClass = TClass; \
	inline static UClass* StaticClass() \
	{ \
		static std::unique_ptr<UClass, UClassDeleter> StaticClassInfo = nullptr; \
		if (!StaticClassInfo) \
		{ \
			constexpr size_t ClassSize = sizeof(UClass); \
			UClass* RawMemory = reinterpret_cast<UClass*>(FPlatformMemory::Malloc<EAT_Object>(ClassSize)); \
			UClass* ClassPtr = new(RawMemory) UClass(#TClass, TSuperClass::StaticClass() ); \
			StaticClassInfo = std::unique_ptr<UClass, UClassDeleter>(ClassPtr, UClassDeleter{}); \
		} \
		return StaticClassInfo.get(); \
	} \