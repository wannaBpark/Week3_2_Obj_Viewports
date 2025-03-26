#include "ObjectHash.h"

#include <span>

#include <assert.h>
#include "Core/Engine.h"
#include "Object/UObject.h"
#include "Debug/DebugConsole.h"
#include "Object/UClass.h"

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

    /** 이 항상 비어있는 집합은, 버킷이 TSet을 사용하지 않는 경우(단 1개의 요소만 있는 경우)에 이터레이터를 얻기 위해 사용됩니다. */
    static TSet<UObject*> EmptyBucket;

    /*
    * 만약 두 포인터 모두 null이라면, 이 버킷은 비어있음을 의미합니다.
    * 첫 번째 포인터는 null이지만 두 번째 포인터가 null이 아니라면, 두 번째 포인터는 TSet 포인터입니다.
    * 첫 번째 포인터가 null이 아니라면, 그것은 UObject 포인터이며 두 번째 포인터는 null이거나 두 번째 요소를 가리킵니다.
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

    /** 생성자 */
    FORCEINLINE FHashBucket()
    {
        ElementsOrSetPtr[0] = nullptr;
        ElementsOrSetPtr[1] = nullptr;
    }
    
    FORCEINLINE ~FHashBucket()
    {
        delete GetSet();
    }
    
    /** 버킷에 객체를 추가합니다. */
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
            // checkSlow(!ElementsOrSetPtr[1]);
        }
    }
    
    /** 버킷에서 객체를 제거합니다. */
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
                // checkSlow((bool)It);
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
    
    /** 이 버킷에 객체가 존재하는지 확인합니다. */
    FORCEINLINE bool Contains(UObject* Object) const
    {
        const TSet<UObject*>* Items = GetSet();
        if (Items)
        {
            return (Items->Find(Object) != Items->end());
        }
        return Object == ElementsOrSetPtr[0] || Object == ElementsOrSetPtr[1];
    }
    
    /** 이 버킷에 포함된 객체의 개수를 반환합니다. */
    FORCEINLINE int32 Num() const
    {
        const TSet<UObject*>* Items = GetSet();
        if (Items)
        {
            return Items->Num();
        }
        return !!ElementsOrSetPtr[0] + !!ElementsOrSetPtr[1];
    }
    
    /** Items TSet이 사용하는 메모리와 그에 할당된 메모리 크기를 반환합니다. */
    FORCEINLINE SIZE_T GetAllocatedSize() const
    {
        const TSet<UObject*>* Items = GetSet();
        if (Items)
        {
            return sizeof(*Items) + Items->GetAllocatedSize();
        }
        return 0;
    }
    
    /** TSet의 Compact()을 호출하여 메모리 사용을 최적화합니다. */
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
    /** Items가 null인 경우, EmptyBucket에 대한 이터레이터를 반환합니다. */
    FORCEINLINE TSet<UObject*>::Iterator GetIteratorForSet()
    {
        TSet<UObject*>* Items = GetSet();
        return Items ? Items->CreateIterator() : EmptyBucket.CreateIterator();
    }
};
TSet<UObject*> FHashBucket::EmptyBucket;

/** 해시 버킷 이터레이터. 버킷 내의 모든 객체를 순회합니다. */
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
	/** 이터레이터를 다음 요소로 진행시킵니다. */
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
	/** 이터레이터가 유효하면 true를 반환하는 bool 형으로의 변환. */
	FORCEINLINE explicit operator bool() const
	{
		if (bItems)
		{
			return (SetIterator.operator->() != nullptr);
		}
		else
		{
			return !bReachedEndNoItems;
		}
	}
	/** "bool" 연산자의 반대 (역) 동작. */
	FORCEINLINE bool operator !() const
	{
		return !static_cast<bool>(*this);
	}
	FORCEINLINE UObject*& operator*()
	{
		if (bItems)
		{
			return const_cast<UObject*&>(*SetIterator);
		}
		else
		{
			return reinterpret_cast<UObject*&>(Bucket.ElementsOrSetPtr[!!bSecondItem]);
		}
	}
};

FORCEINLINE FHashBucketIterator FHashBucket::CreateIterator()
{
	return FHashBucketIterator(*this);
}

/**
 * 읽기 전용 잠금을 지원하는, FHashBucket 값을 갖는 TMap의 래퍼 클래스
 */
template <typename T, typename K = FHashBucket>
class TBucketMap : private TMap<T, K>
{
	typedef TMap<T, K> Super;
	int32 ReadOnlyLock = 0;
public:

	using Super::begin;
	using Super::end;
	using Super::GetAllocatedSize;
	using Super::Find;
	using Super::Num;
	using Super::FindChecked;

	FORCEINLINE void LockReadOnly()
	{
		ReadOnlyLock++;
	}
	FORCEINLINE void UnlockReadOnly()
	{
		ReadOnlyLock--;
		// check(ReadOnlyLock >= 0);
	}

	FORCEINLINE void Compact()
	{
		Super::Compact();
	}

	FORCEINLINE void Add(const T& Key)
	{
		Super::Add(Key);
	}

	FORCEINLINE void Remove(const T& Key)
	{
		Super::Remove(Key);
	}

	FORCEINLINE K& FindOrAdd(const T& Key)
	{
		return Super::FindOrAdd(Key);
	}

	FORCEINLINE K& FindOrAdd(T&& Key)
	{
		return Super::FindOrAdd(Key);
	}
};

/**
 * 간단한 BucketMapLock
 */
template <typename T>
struct TBucketMapLock
{
	T& Map;
	explicit FORCEINLINE TBucketMapLock(T& InMap)
		: Map(InMap)
	{
		Map.LockReadOnly();
	}
	FORCEINLINE ~TBucketMapLock()
	{
		Map.UnlockReadOnly();
	}
};

class FUObjectHashTables
{
	// /** 여러 스레드에서 동시에 추가되는 작업을 방지하는 임계 구역 */
	// FCriticalSection CriticalSection;

public:

	/** 해시 집합들 */
	TBucketMap<int32> Hash;
	TMultiMap<int32, uint32> HashOuter;

	/** 객체와 해당 Outer를 매핑한 맵. 객체 이터레이터를 사용하지 않고 Outer 정보를 찾기 위해 사용됩니다. */
	TBucketMap<UObject*> ObjectOuterMap;
	TBucketMap<UClass*> ClassToObjectListMap;

	/** 각 UClass*를 키로 하고, 해당 클래스의 자식 클래스 집합을 값으로 갖는 맵 */
	TMap<UClass*, TSet<UClass*>> ClassToChildListMap;
	std::atomic<uint64> AllClassesVersion;
	std::atomic<uint64> NativeClassesVersion;

	// TODO : Package 추가 시
	// /** 패키지와 그 안에 포함된 객체들을 매핑한 맵 */
	// TBucketMap<UPackage*> PackageToObjectListMap;
	// /** 객체와 해당 외부 패키지를 매핑한 맵 */
	// TMap<UObject*, UPackage*> ObjectToPackageMap;

	FUObjectHashTables()
		: AllClassesVersion(0)
		, NativeClassesVersion(0)
	{
	}

	void ShrinkMaps()
	{
		// case 0
		Hash.Compact();
		for (auto& Pair : Hash)
		{
			Pair.Value.Shrink();
		}

		// case 1
		HashOuter.Compact();

		// case 2
		ObjectOuterMap.Compact();
		for (auto& Pair : ObjectOuterMap)
		{
			Pair.Value.Shrink();
		}

		// case 3
		ClassToObjectListMap.Compact();
		for (auto& Pair : ClassToObjectListMap)
		{
			Pair.Value.Shrink();
		}

		// case 4
		ClassToChildListMap.Compact();
		for (auto& Pair : ClassToChildListMap)
		{
			Pair.Value.Shrink();
		}

		// // case 5
		// PackageToObjectListMap.Compact();
		// for (auto& Pair : PackageToObjectListMap)
		// {
		// 	Pair.Value.Shrink();
		// }
		//
		// // case 6
		// ObjectToPackageMap.Compact();
	}

	/** FName 해시 테이블에 Hash/Object 쌍이 존재하는지 확인합니다. */
	FORCEINLINE bool PairExistsInHash(int32 InHash, UObject* Object)
	{
		bool bResult = false;
		FHashBucket* Bucket = Hash.Find(InHash);
		if (Bucket)
		{
			bResult = Bucket->Contains(Object);
		}
		return bResult;
	}
	/** FName 해시 테이블에 Hash/Object 쌍을 추가합니다. */
	FORCEINLINE void AddToHash(int32 InHash, UObject* Object)
	{
		FHashBucket& Bucket = Hash.FindOrAdd(InHash);
		Bucket.Add(Object);
	}
	/** FName 해시 테이블에서 Hash/Object 쌍을 제거합니다. */
	FORCEINLINE int32 RemoveFromHash(int32 InHash, UObject* Object)
	{
		int32 NumRemoved = 0;
		FHashBucket* Bucket = Hash.Find(InHash);
		if (Bucket)
		{
			NumRemoved = Bucket->Remove(Object);
			if (Bucket->Num() == 0)
			{
				Hash.Remove(InHash);
			}
		}
		return NumRemoved;
	}

	// FORCEINLINE void Lock()
	// {
	// 	CriticalSection.Lock();
	// }
	//
	// FORCEINLINE void Unlock()
	// {
	// 	CriticalSection.Unlock();
	// }

	static FUObjectHashTables& Get()
	{
		static FUObjectHashTables Singleton;
		return Singleton;
	}
};


static FORCEINLINE int32 GetObjectHash(FName ObjName)
{
	return ObjName.ComparisonIndex + ObjName.DisplayIndex;
}

void AddToHashMap(UObject* Object)
{
	FUObjectHashTables::Get().AddToHash(Object->GetClass()->GetFName().GetComparisonIndex(), Object);
}

void RemoveFromHashMap(UObject* Object)
{
	FUObjectHashTables::Get().RemoveFromHash(Object->GetClass()->GetFName().GetComparisonIndex(), Object);
}

void GetObjectsOfClass(const UClass* ClassToLookFor, TArray<UObject*>& Results, bool bIncludeDerivedClasses, EObjectFlags ExclusionFlags, EInternalObjectFlags ExclusionInternalFlags)
{
	ForEachObjectOfClass(ClassToLookFor,
		[&Results](UObject* Object)
		{
			Results.Add(Object);
		}
	, bIncludeDerivedClasses, ExclusionFlags, ExclusionInternalFlags);

	static_assert(Results.Num() <= UEngine::Get().GObjects.Num()); // otherwise we have a cycle in the outer chain, which should not be possible
}


/**
지정된 클래스의 객체들을 순회하며 각 객체에 대해 Operation을 실행합니다.

@tparam Func 호출 가능한 객체(람다, 함수 포인터 등)의 타입.
@param ClassToLookFor 찾고자 하는 객체의 기준 클래스.
@param Operation 각 객체에 대해 실행할 호출 가능한 객체.
@param bIncludeDerivedClasses true이면, 기준 클래스의 파생 클래스 객체들도 순회합니다.
@param ExcludeFlags 추가로 제외할 UObject 플래그.
@param ExclusionInternalFlags 제외에 사용할 내부 객체 플래그.
*/ 
template<typename ClassType, typename ArrayAllocator>
static void RecursivelyPopulateDerivedClasses(FUObjectHashTables& ThreadHash, const UClass* ParentClass,
                                              TArray<ClassType, ArrayAllocator>& OutAllDerivedClass)
{
	// 검색을 시작할 때, 가상 인덱스 (OutAllDerivedClass.Num() - 1)에 부모 클래스부터 시작하고,
	// 이후에 요소가 추가됨에 따라 인덱스 OutAllDerivedClass.Num()부터 계속 검색합니다.
	int32 SearchIndex = OutAllDerivedClass.Num() - 1;
	const UClass* SearchClass = ParentClass;

	while (true)
	{
		TSet<UClass*>* ChildSet = ThreadHash.ClassToChildListMap.Find(const_cast<UClass*>(SearchClass));
		if (ChildSet)
		{
			OutAllDerivedClass.Reserve(OutAllDerivedClass.Num() + ChildSet->Num());
			for (UClass* ChildClass : *ChildSet)
			{
				OutAllDerivedClass.Add(ChildClass);
			}
		}

		// 위의 코드에 의해 채워졌다면, 이제 다음 인덱스에서 검색을 계속합니다.
		++SearchIndex;

		if (SearchIndex < OutAllDerivedClass.Num())
		{
			SearchClass = OutAllDerivedClass[SearchIndex];			
		}
		else
		{
			return;
		}
	}
}

/**
 * 주어진 클래스 목록(ClassesToLookFor)에 해당하는 모든 객체들을 순회하며 각 객체에 대해 Operation을 실행합니다.
 *
 * @tparam Func 호출 가능한 객체(람다, 함수 포인터 등)의 타입.
 * @param ThreadHash 객체 해시 테이블(FUObjectHashTables)의 인스턴스.
 * @param ClassesToLookFor 검색할 클래스들의 연속된 메모리 뷰. (std::span을 사용하여 비소유 참조)
 * @param Operation 각 객체에 대해 실행할 호출 가능한 객체.
 * @param ExcludeFlags 결과에서 제외할 추가적인 UObject 플래그 (기본값: RF_ClassDefaultObject).
 * @param ExclusionInternalFlags 결과에서 제외할 내부 객체 플래그 (기본값: 없음).
 */
template<typename Func>
FORCEINLINE void ForEachObjectOfClasses_Implementation(
	FUObjectHashTables& ThreadHash,
	const std::span<const UClass*> ClassesToLookFor,
	Func Operation,
	EObjectFlags ExcludeFlags = RF_ClassDefaultObject,
	EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None)
{
	// 필요한 플래그 설정
	ExclusionInternalFlags |= EInternalObjectFlags::Unreachable | EInternalObjectFlags_AsyncLoading;

	TBucketMapLock ClassToObjectListMapLock(ThreadHash.ClassToObjectListMap);

	for (const UClass* SearchClass : ClassesToLookFor)
	{
		UClass* SearchKey = const_cast<UClass*>(SearchClass);
		const auto List = ThreadHash.ClassToObjectListMap.Find(SearchKey);

		if (List)
		{
			for (auto ObjectIt = List->CreateIterator(); ObjectIt; ++ObjectIt)
			{
				UObject* Object = static_cast<UObject*>(*ObjectIt);
				// TODO : GC 관련?
				// if (!Object->HasAnyFlags(ExcludeFlags) && !Object->HasAnyInternalFlags(ExclusionInternalFlags))
				// {
				// 	if (UE::GC::GIsIncrementalReachabilityPending)
				// 	{
				// 		UE::GC::MarkAsReachable(Object);
				// 	}
				// }
				Operation(Object);
			}
		}
	}
}

template<typename Func>
void ForEachObjectOfClass(const UClass* ClassToLookFor, Func Operation, const bool bIncludeDerivedClasses, EObjectFlags ExcludeFlags, EInternalObjectFlags ExclusionInternalFlags)
{
	// 대부분의 클래스는 약 10개의 서브클래스를 가지며, 어떤 클래스는 수백 개의 서브클래스를 가집니다.
	TArray<const UClass*> ClassesToSearch;
	ClassesToSearch.Add(ClassToLookFor);
	
	FUObjectHashTables& ThreadHash = FUObjectHashTables::Get();
	/* 지금은 Lock 필요 없음 */
	// FHashTableLock HashLock(ThreadHash);

	if (bIncludeDerivedClasses)
	{
		RecursivelyPopulateDerivedClasses(ThreadHash, ClassToLookFor, ClassesToSearch);
	}

	std::span<const UClass*> arrayView(ClassesToSearch);
	ForEachObjectOfClasses_Implementation(ThreadHash, arrayView, Operation, ExcludeFlags, ExclusionInternalFlags);
}

template<typename Func>
void ForEachObjectOfClasses(TArray<const UClass* const> ClassesToLookFor, Func Operation, EObjectFlags ExcludeFlags, EInternalObjectFlags ExclusionInternalFlags)
{
	FUObjectHashTables& ThreadHash = FUObjectHashTables::Get();
	/* 지금은 Lock 필요 없음 */
	// FHashTableLock HashLock(ThreadHash);

	ForEachObjectOfClasses_Implementation(ThreadHash, ClassesToLookFor, Operation, ExcludeFlags, ExclusionInternalFlags);
}

void GetDerivedClasses(const UClass* ClassToLookFor, TArray<UClass*>& Results, bool bRecursive)
{
	FUObjectHashTables& ThreadHash = FUObjectHashTables::Get();
	/* 지금은 Lock 필요 없음 */
	// FHashTableLock HashLock(ThreadHash);

	if (bRecursive)
	{
		RecursivelyPopulateDerivedClasses(ThreadHash, ClassToLookFor, Results);
	}
	else
	{
		TSet<UClass*>* DerivedClasses = ThreadHash.ClassToChildListMap.Find(const_cast<UClass*>(ClassToLookFor));
		if (DerivedClasses)
		{
			Results.Reserve(Results.Num() + DerivedClasses->Num());
			for (UClass* DerivedClass : *DerivedClasses)
			{
				Results.Add(DerivedClass);
			}
		}
	}
}

TMap<UClass*, TSet<UClass*>> GetAllDerivedClasses()
{
	FUObjectHashTables& ThreadHash = FUObjectHashTables::Get();
	// FHashTableLock HashLock(ThreadHash);
	return ThreadHash.ClassToChildListMap;
}

void ShrinkUObjectHashTables()
{
	FUObjectHashTables& ThreadHash = FUObjectHashTables::Get();
	//FHashTableLock HashLock(ThreadHash);
	ThreadHash.ShrinkMaps();
}

