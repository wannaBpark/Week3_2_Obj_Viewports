#pragma once
#include "functional"
#include "Core/Container/String.h"
#include "Core/Container/CString.h"
#include "Object/NameTypes.h"
#include "Core/Container/Array.h"
#include "Core/Container/Map.h"
#include "Core/Container/Set.h"
#include "Core/HAL/PlatformType.h"
#include "Core/EngineTypes.h"

class UClass;
class UObject;

UObject* StaticFindObjectFastInternal(const UClass* Class, const UObject* InOuter, FName InName, bool ExactClass, bool AnyPackage, EObjectFlags ExclusiveFlags = RF_NoFlags, EInternalObjectFlags ExclusiveInternalFlags = EInternalObjectFlags::None);

UObject* StaticFindObjectFastInternal(const UClass* Class, const UObject* InOuter, FName InName, bool ExactClass = false, EObjectFlags ExclusiveFlags = RF_NoFlags, EInternalObjectFlags ExclusiveInternalFlags = EInternalObjectFlags::None);

bool StaticFindAllObjectsFastInternal(TArray<UObject*>& OutFoundObjects, const UClass* ObjectClass, FName ObjectName, bool bExactClass, EObjectFlags ExcludeFlags = RF_NoFlags, EInternalObjectFlags ExclusiveInternalFlags = EInternalObjectFlags::None);

UObject* StaticFindFirstObjectFastInternal(const UClass* ObjectClass, FName ObjectName, bool bExactClass, EObjectFlags ExcludeFlag = RF_NoFlags, EInternalObjectFlags ExclusiveInternalFlags = EInternalObjectFlags::None);

UObject* StaticFindObjectFastExplicit(const UClass* ObjectClass, FName ObjectName, const FString& ObjectPathName, bool bExactClass, EObjectFlags ExcludeFlags = RF_NoFlags);

void GetObjectsWithOuter(const class UObject* Outer, TArray<UObject*>& Results, bool bIncludeNestedObjects = true, EObjectFlags ExclusionFlags = RF_NoFlags, EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None);

void ForEachObjectWithOuterBreakable(const class UObject* Outer, std::function<bool(UObject*)> Operation, bool bIncludeNestedObjects = true, EObjectFlags ExclusionFlags = RF_NoFlags, EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None);

inline void ForEachObjectWithOuter(const class UObject* Outer, std::function<void(UObject*)> Operation, bool bIncludeNestedObjects = true, EObjectFlags ExclusionFlags = RF_NoFlags, EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None)
{
	ForEachObjectWithOuterBreakable(Outer, [Operation](UObject* Object) { Operation(Object); return true; }, bIncludeNestedObjects, ExclusionFlags, ExclusionInternalFlags);
}

class UObject* FindObjectWithOuter(const class UObjectBase* Outer, const class UClass* ClassToLookFor = nullptr, FName NameToLookFor = NAME_None);

void GetObjectsOfClass(const UClass* ClassToLookFor, TArray<UObject*>& Results, bool bIncludeDerivedClasses = true, EObjectFlags ExcludeFlags = RF_ClassDefaultObject, EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None);

void ForEachObjectOfClass(const UClass* ClassToLookFor, std::function<void(UObject*)> Operation, bool bIncludeDerivedClasses = true, EObjectFlags ExcludeFlags = RF_ClassDefaultObject, EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None);

void ForEachObjectOfClasses(TArray<const UClass* const> ClassesToLookFor, std::function<void(UObject*)> Operation, EObjectFlags ExcludeFlags = RF_ClassDefaultObject, EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None);

void GetDerivedClasses(const UClass* ClassToLookFor, TArray<UClass*>& Results, bool bRecursive = true);

TMap<UClass*, TSet<UClass*>> GetAllDerivedClasses();

void HashObject(class UObjectBase* Object);

void UnhashObject(class UObjectBase* Object);

void ShrinkUObjectHashTables();

//UPackage* GetObjectExternalPackageThreadSafe(const class UObjectBase* Object);
//
//UPackage* GetObjectExternalPackageInternal(const class UObjectBase* Object);

void LockUObjectHashTables();

void UnlockUObjectHashTables();

class FScopedUObjectHashTablesLock
{
public:
	FORCEINLINE FScopedUObjectHashTablesLock()
	{
		LockUObjectHashTables();
	}
	FORCEINLINE ~FScopedUObjectHashTablesLock()
	{
		UnlockUObjectHashTables();
	}
};


class UObjectHash
{
};

