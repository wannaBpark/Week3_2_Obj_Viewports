#include "ObjectHash.h"
#include "assert.h"
#include "Core/Engine.h"
#include "Object/UObject.h"
#include "Debug/DebugConsole.h"

static inline void OnHashFailure(UObject* Object, const TCHAR* HashName, const TCHAR* FailureKind)
{
    UE_LOG(TEXT("UObject %s consistency failure (%s). Checking for memory corruption"), HashName, FailureKind);
   
	if (Object == nullptr)
	{
		UE_LOG(TEXT("Object is null"));
	}

	UClass* Class = Object->GetClass();
    UE_LOG(TEXT("Unidentified failure for object %s, hash itself may be corrupted or buggy."), Object->GetFName().ToString());
}

struct FHashBucket
{
	friend struct FHashBucketIterator;

	/** This always empty set is used to get an iterator if the bucket doesn't use a TSet (has only 1 element) */
	static TSet<UObject*> EmptyBucket;

	/*
	* If these are both null, this bucket is empty
	* If the first one is null, but the second one is non-null, then the second one is a TSet pointer
	* If the first one is not null, then it is a uobject ptr, and the second ptr is either null or a second element
	*/
	void* ElementsOrSetPtr[2];

	FORCEINLINE TSet<UObject*>* GetSet()
	{
		if (ElementsOrSetPtr[1] && !ElementsOrSetPtr[0])
		{
			return (TSet<UObject*>*)ElementsOrSetPtr[1];
		}
		return nullptr;
	}

	FORCEINLINE const TSet<UObject*>* GetSet() const
	{
		if (ElementsOrSetPtr[1] && !ElementsOrSetPtr[0])
		{
			return (TSet<UObject*>*)ElementsOrSetPtr[1];
		}
		return nullptr;
	}

	/** Constructor */
	FORCEINLINE FHashBucket()
	{
		ElementsOrSetPtr[0] = nullptr;
		ElementsOrSetPtr[1] = nullptr;
	}
	FORCEINLINE ~FHashBucket()
	{
		delete GetSet();
	}
	/** Adds an Object to the bucket */
	FORCEINLINE void Add(UObject* Object)
	{
		TSet<UObject*>* Items = GetSet();
		if (Items)
		{
			Items->Add(Object);
		}
		else if (ElementsOrSetPtr[0] && ElementsOrSetPtr[1])
		{
			Items = new TSet<UObject*>();
			Items->Add((UObject*)ElementsOrSetPtr[0]);
			Items->Add((UObject*)ElementsOrSetPtr[1]);
			Items->Add(Object);
			ElementsOrSetPtr[0] = nullptr;
			ElementsOrSetPtr[1] = Items;
		}
		else if (ElementsOrSetPtr[0])
		{
			ElementsOrSetPtr[1] = Object;
		}
		else
		{
			ElementsOrSetPtr[0] = Object;
			__assume(!ElementsOrSetPtr[1]);
		}
	}
	/** Removes an Object from the bucket */
	FORCEINLINE int32 Remove(UObject* Object)
	{
		int32 Result = 0;
		TSet<UObject*>* Items = GetSet();
		if (Items)
		{
			Result = Items->Remove(Object);
			if (Items->Num() <= 2)
			{
				auto It = Items->begin();
				ElementsOrSetPtr[0] = *It;
				__assume(It != Items->end());
				++It;
				ElementsOrSetPtr[1] = *It;
				delete Items;
			}
		}
		else if (Object == ElementsOrSetPtr[1])
		{
			Result = 1;
			ElementsOrSetPtr[1] = nullptr;
		}
		else if (Object == ElementsOrSetPtr[0])
		{
			Result = 1;
			ElementsOrSetPtr[0] = ElementsOrSetPtr[1];
			ElementsOrSetPtr[1] = nullptr;
		}
		return Result;
	}
	/** Checks if an Object exists in this bucket */
	FORCEINLINE bool Contains(UObject* Object) const
	{
		const TSet<UObject*>* Items = GetSet();
		if (Items)
		{
			return (Items->Find(Object) != Items->end());
		}
		return Object == ElementsOrSetPtr[0] || Object == ElementsOrSetPtr[1];
	}
	/** Returns the number of Objects in this bucket */
	FORCEINLINE int32 Num() const
	{
		const TSet<UObject*>* Items = GetSet();
		if (Items)
		{
			return Items->Num();
		}
		return !!ElementsOrSetPtr[0] + !!ElementsOrSetPtr[1];
	}
	/** Returns the amount of memory allocated for and by Items TSet */
	FORCEINLINE SIZE_T GetAllocatedSize() const
	{
		const TSet<UObject*>* Items = GetSet();
		if (Items)
		{
			return sizeof(*Items) + Items->GetAllocatedSize();
		}
		return 0;
	}
	void Shrink()
	{
		TSet<UObject*>* Items = GetSet();
		if (Items)
		{
			Items->Compact();
		}
	}
	FORCEINLINE struct FHashBucketIterator CreateIterator();

private:
	/** Gets an iterator for the TSet in this bucket or for the EmptyBucker if Items is null */
	FORCEINLINE TSet<UObject*>::Iterator GetIteratorForSet()
	{
		TSet<UObject*>* Items = GetSet();
		return Items ? Items->CreateIterator() : EmptyBucket.CreateIterator();
	}
};
TSet<UObject*> FHashBucket::EmptyBucket;

/** Hash Bucket Iterator. Iterates over all Objects in the bucket */
struct FHashBucketIterator
{
	FHashBucket& Bucket;
	TSet<UObject*>::Iterator SetIterator;
	bool bItems;
	bool bReachedEndNoItems;
	bool bSecondItem;

	FORCEINLINE FHashBucketIterator(FHashBucket& InBucket)
		: Bucket(InBucket)
		, SetIterator(InBucket.GetIteratorForSet())
		, bItems(!!InBucket.GetSet())
		, bReachedEndNoItems(!InBucket.ElementsOrSetPtr[0] && !InBucket.ElementsOrSetPtr[1])
		, bSecondItem(false)
	{
	}
	/** Advances the iterator to the next element. */
	FORCEINLINE FHashBucketIterator& operator++()
	{
		if (bItems)
		{
			++SetIterator;
		}
		else
		{
			bReachedEndNoItems = bSecondItem || !Bucket.ElementsOrSetPtr[1];
			bSecondItem = true;
		}
		return *this;
	}
	/** conversion to "bool" returning true if the iterator is valid. */
	FORCEINLINE explicit operator bool() const
	{
		if (bItems)
		{
			return(SetIterator != nullptr);
		}
		else
		{
			return !bReachedEndNoItems;
		}
	}
	/** inverse of the "bool" operator */
	FORCEINLINE bool operator !() const
	{
		return !(bool)*this;
	}
	FORCEINLINE UObject*& operator*()
	{
		if (bItems)
		{
			return *SetIterator;
		}
		else
		{
			return (UObject*&)Bucket.ElementsOrSetPtr[!!bSecondItem];
		}
	}
};

UObject* StaticFindObjectFastInternal(const UClass* ObjectClass, const UObject* ObjectPackage, FName InName, bool ExactClass, bool AnyPackage, EObjectFlags ExclusiveFlags, EInternalObjectFlags ExclusiveInternalFlags)
{
	UObject* Result = nullptr;

    //원자적으로 되야한다 함
    {
        //assert(ObjectPackage != ANY_PACKAGE_DEPRECATED); // this could never have returned anything but nullptr
        /*
        FUObejctHashTable& ThreadHash = FUObejctHashTable::Get();
        Result = StaticFindObjectFastInternal(ThreadHash, )*/
    }
}

UObject* StaticFindObjectFastInternal(const UClass* Class, const UObject* InOuter, FName InName, bool ExactClass, EObjectFlags ExclusiveFlags, EInternalObjectFlags ExclusiveInternalFlags)
{
    return nullptr;
}

bool StaticFindAllObjectsFastInternal(TArray<UObject*>& OutFoundObjects, const UClass* ObjectClass, FName ObjectName, bool bExactClass, EObjectFlags ExcludeFlags, EInternalObjectFlags ExclusiveInternalFlags)
{
    return false;
}

UObject* StaticFindFirstObjectFastInternal(const UClass* ObjectClass, FName ObjectName, bool bExactClass, EObjectFlags ExcludeFlag, EInternalObjectFlags ExclusiveInternalFlags)
{
    return nullptr;
}

UObject* StaticFindObjectFastExplicit(const UClass* ObjectClass, FName ObjectName, const FString& ObjectPathName, bool bExactClass, EObjectFlags ExcludeFlags)
{
    return nullptr;
}

void GetObjectsWithOuter(const UObject* Outer, TArray<UObject*>& Results, bool bIncludeNestedObjects, EObjectFlags ExclusionFlags, EInternalObjectFlags ExclusionInternalFlags)
{
}

void ForEachObjectWithOuterBreakable(const UObject* Outer, std::function<bool(UObject*)> Operation, bool bIncludeNestedObjects, EObjectFlags ExclusionFlags, EInternalObjectFlags ExclusionInternalFlags)
{
}

UObject* FindObjectWithOuter(const UObject* Outer, const UClass* ClassToLookFor, FName NameToLookFor)
{
    return nullptr;
}

void GetObjectsOfClass(const UClass* ClassToLookFor, TArray<UObject*>& Results, bool bIncludeDerivedClasses, EObjectFlags ExcludeFlags, EInternalObjectFlags ExclusionInternalFlags)
{
}

void ForEachObjectOfClass(const UClass* ClassToLookFor, std::function<void(UObject*)> Operation, bool bIncludeDerivedClasses, EObjectFlags ExcludeFlags, EInternalObjectFlags ExclusionInternalFlags)
{
}

void ForEachObjectOfClasses(TArray<const UClass* const> ClassesToLookFor, std::function<void(UObject*)> Operation, EObjectFlags ExcludeFlags, EInternalObjectFlags ExclusionInternalFlags)
{
}

void GetDerivedClasses(const UClass* ClassToLookFor, TArray<UClass*>& Results, bool bRecursive)
{
}

void HashObject(UObject* Object)
{
}

void UnhashObject(UObject* Object)
{
}

void ShrinkUObjectHashTables()
{
}

void LockUObjectHashTables()
{
}

void UnlockUObjectHashTables()
{
}

FHashBucketIterator FHashBucket::CreateIterator()
{
	return FHashBucketIterator(*this);
}
