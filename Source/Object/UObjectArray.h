#pragma once
#include "atomic"
#include "assert.h"
#include "UObject.h"
#include "Core/Container/Array.h"
#include "Core/EngineTypes.h"

struct FUObjectItem
{
	friend class FUObjectArray;
	class UObject* Object;
private:
	std::atomic<int32> Flags;
public:
	std::atomic<int32> ClusterRootIndex;
	std::atomic<int32> SerialNumber;
	std::atomic<int32> RefCount;

public:
	FUObjectItem()
		: Object(nullptr)
		, Flags(0)
		, ClusterRootIndex(INDEX_NONE)
		, SerialNumber(0)
		, RefCount(0)
	{
	}

	~FUObjectItem()
	{
		Object = nullptr;
	}

	FUObjectItem(FUObjectItem&&) = delete;
	FUObjectItem(const FUObjectItem&) = delete;
	FUObjectItem& operator=(FUObjectItem&&) = delete;
	FUObjectItem& operator=(const FUObjectItem&) = delete;

	FORCEINLINE void SetOwnerIndex(int32 OwnerIndex)
	{
		ClusterRootIndex = OwnerIndex;
	}

	FORCEINLINE int32 GetOwnerIndex() const
	{
		return ClusterRootIndex;
	}

	/** Encodes the cluster index in the ClusterRootIndex variable */
	FORCEINLINE void SetClusterIndex(int32 ClusterIndex)
	{
		ClusterRootIndex = -ClusterIndex - 1;
	}

	/** Decodes the cluster index from the ClusterRootIndex variable */
	FORCEINLINE int32 GetClusterIndex() const
	{
		__assume(ClusterRootIndex < 0);
		return -ClusterRootIndex - 1;
	}

	FORCEINLINE int32 GetSerialNumber() const
	{
		return SerialNumber;
	}

	FORCEINLINE void SetFlags(EInternalObjectFlags FlagsToSet)
	{
		ThisThreadAtomicallySetFlag(FlagsToSet);
	}

	FORCEINLINE EInternalObjectFlags GetFlags() const
	{
		return EInternalObjectFlags(GetFlagsInternal());
	}

	FORCEINLINE void ClearFlags(EInternalObjectFlags FlagsToClear)
	{
		ThisThreadAtomicallyClearedFlag(FlagsToClear);
	}

	/**
	 * Uses atomics to clear the specified flag(s). GC internal version
	 * @param FlagsToClear
	 * @return True if this call cleared the flag, false if it has been cleared by another thread.
	 */
	FORCEINLINE bool ThisThreadAtomicallyClearedFlag_ForGC(EInternalObjectFlags FlagToClear)
	{
		return AtomicallyClearFlag_ForGC(FlagToClear);
	}

	/**
	 * Uses atomics to clear the specified flag(s).
	 * @param FlagsToClear
	 * @return True if this call cleared the flag, false if it has been cleared by another thread.
	 */
	FORCEINLINE bool ThisThreadAtomicallyClearedFlag(EInternalObjectFlags FlagToClear)
	{
		bool Result = false;
		FlagToClear &= ~EInternalObjectFlags_ReachabilityFlags;
		FlagToClear |= EInternalObjectFlags::RefCounted;
		if (!!(FlagToClear &EInternalObjectFlags_RootFlags))
		{
			Result = ClearRootFlags(FlagToClear);
		}
		else
		{
			Result = AtomicallyClearFlag_ForGC(FlagToClear);
		}
		return Result;
	}

	/**
	 * Uses atomics to set the specified flag(s). GC internal version.
	 * @param FlagToSet
	 * @return True if this call set the flag, false if it has been set by another thread.
	 */

	FORCEINLINE bool ThisThreadAtomicallySetFlag_ForGC(EInternalObjectFlags FlagToSet)
	{
		return AtomicallySetFlag_ForGC(FlagToSet);
	}

	/**
	 * Uses atomics to set the specified flag(s)
	 * @param FlagToSet
	 * @return True if this call set the flag, false if it has been set by another thread.
	 */
	FORCEINLINE bool ThisThreadAtomicallySetFlag(EInternalObjectFlags FlagToSet)
	{
		bool Result = false;
		FlagToSet &= ~EInternalObjectFlags_ReachabilityFlags; // reachability flags can only be cleared by GC through *_ForGC functions
		FlagToSet &= ~EInternalObjectFlags::RefCounted; // refcounted flag is internal and must only be set by AddRef/ReleaseRef.
		if (!!(FlagToSet & EInternalObjectFlags_RootFlags))
		{
			Result = SetRootFlags(FlagToSet);
		}
		else
		{
			Result = AtomicallySetFlag_ForGC(FlagToSet);
		}
		return Result;
	}

	FORCEINLINE bool HasAnyFlags(EInternalObjectFlags InFlags) const
	{
		return !!(GetFlagsInternal() & int32(InFlags));
	}

	FORCEINLINE bool HasAllFlags(EInternalObjectFlags InFlags) const
	{
		return (GetFlagsInternal() & int32(InFlags)) == int32(InFlags);
	}

	FORCEINLINE void SetUnreachable()
	{
		AtomicallySetFlag_ForGC(EInternalObjectFlags::Unreachable);
	}

	void SetMaybeUnreachable();

	FORCEINLINE void ClearUnreachable()
	{
		AtomicallyClearFlag_ForGC(EInternalObjectFlags::Unreachable);
	}

	FORCEINLINE bool IsUnreachable() const
	{
		return !!(GetFlagsInternal() & static_cast<int32>(EInternalObjectFlags::Unreachable));
	}

	bool IsMaybeUnreachable() const;

	FORCEINLINE bool ThisThreadAtomicallyClearedRFUnreachable()
	{
		return AtomicallyClearFlag_ForGC(EInternalObjectFlags::Unreachable);
	}
	FORCEINLINE void SetGarbage()
	{
		AtomicallySetFlag_ForGC(EInternalObjectFlags::Garbage);
	}
	FORCEINLINE void ClearGarbage()
	{
		AtomicallyClearFlag_ForGC(EInternalObjectFlags::Garbage);
	}
	FORCEINLINE bool IsGarbage() const
	{
		return !!(GetFlagsInternal() & static_cast<int32>(EInternalObjectFlags::Garbage));
	}

	FORCEINLINE void SetPendingKill()
	{
		SetGarbage();
	}

	FORCEINLINE void ClearPendingKill()
	{
		ClearGarbage();
	}

	FORCEINLINE bool IsPendingKill() const
	{
		return IsGarbage();
	}

	FORCEINLINE void SetRootSet()
	{
		ThisThreadAtomicallySetFlag(EInternalObjectFlags::RootSet);
	}
	FORCEINLINE void ClearRootSet()
	{
		ThisThreadAtomicallyClearedFlag(EInternalObjectFlags::RootSet);
	}
	FORCEINLINE bool IsRootSet() const
	{
		return !!(GetFlagsInternal() & static_cast<int32>(EInternalObjectFlags::RootSet));
	}

	FORCEINLINE int32 GetRefCount() const
	{
		return RefCount;
	}

	void AddRef()
	{
		{
			RefCount.fetch_add(1);
			if ((GetFlags() & EInternalObjectFlags::RefCounted) != EInternalObjectFlags::RefCounted)
			{
				SetRootFlags(EInternalObjectFlags::RefCounted);
			}
		};
	}

	void ReleaseRef()
	{
		{
			// This alone is not thread-safe as we may race with AddRef and in that case we don't want ClearRootFlags to apply.
			// We fix this by validating that the refcount is still 0 while inside the root locks in ClearRootFlags.
			const int32 NewRefCount = RefCount.fetch_sub(1);
			if (NewRefCount >= 0)
			{
				if (NewRefCount == 0)
				{
					ClearRootFlags(EInternalObjectFlags::RefCounted);
				}
			}
		};
	}

#if STATS || ENABLE_STATNAMEDEVENTS_UOBJECT
	COREUOBJECT_API void CreateStatID() const;
#endif

	void FastMarkAsReachableInterlocked_ForGC();


	void FastMarkAsReachableAndClearReachaleInClusterInterlocked_ForGC();

	/**
	 * Mark this object item as Reachable and clear MaybeUnreachable flag. Only thread-safe for concurrent clear, not concurrent set+clear. Don't use during mark phase. For GC use only.
	 * @return True if this call cleared MaybeUnreachable flag, false if it has been cleared by another thread.
	 */

	bool MarkAsReachableInterlocked_ForGC();

	FORCEINLINE static constexpr ::size_t OffsetOfFlags()
	{
		return offsetof(FUObjectItem, Flags);
	}

private:
	FORCEINLINE int32 GetFlagsInternal() const
	{
		return Flags.load(std::memory_order_relaxed);
	}

	FORCEINLINE int32 GetRefCountInternal() const
	{
		return RefCount.load(std::memory_order_relaxed);
	}

	bool SetRootFlags(EInternalObjectFlags FlagsToSet);
	bool ClearRootFlags(EInternalObjectFlags FlagsToClear);

	/**
	 * Uses atomics to set the specified flag(s). GC internal version.
	 * @param FlagToSet
	 * @return True if this call set the flag, false if it has been set by another thread.
	 */
	FORCEINLINE bool AtomicallySetFlag_ForGC(EInternalObjectFlags FlagToSet)
	{
		static_assert(sizeof(int32) == sizeof(Flags), "Flags must be 32-bit for atomics.");
		bool bIChangedIt = false;
		while (1)
		{
			int32 StartValue = GetFlagsInternal();
			if ((StartValue & static_cast<int32>(FlagToSet)) == static_cast<int32>(FlagToSet))
			{
				break;
			}
			int32 NewValue = StartValue | static_cast<int32>(FlagToSet);
			int32 expected = StartValue;
			if (Flags.compare_exchange_strong(expected, NewValue, std::memory_order_relaxed))
			{
				bIChangedIt = true;
				break;
			}
		}
		return bIChangedIt;
	}

	/**
	 * Uses atomics to clear the specified flag(s). GC internal version
	 * @param FlagsToClear
	 * @return True if this call cleared the flag, false if it has been cleared by another thread.
	 */
	FORCEINLINE bool AtomicallyClearFlag_ForGC(EInternalObjectFlags FlagToClear)
	{
		static_assert(sizeof(int32) == sizeof(Flags), "Flags must be 32-bit for atomics.");
		bool bIChangedIt = false;
		while (1)
		{
			int32 StartValue = GetFlagsInternal();
			if (!(StartValue & static_cast<int32>(FlagToClear)))
			{
				break;
			}
			int32 NewValue = StartValue & ~static_cast<int32>(FlagToClear);
			// compare_exchange_strong는 expected 값을 참조로 받고, 교체에 실패하면 StartValue에 현재 값을 업데이트합니다.
			if (Flags.compare_exchange_strong(StartValue, NewValue, std::memory_order_relaxed))
			{
				bIChangedIt = true;
				break;
			}
		}
		return bIChangedIt;
	}

};

namespace UObjectArrayPrivate
{
	void FailMaxUObjectCountExceeded(const int32 MaxUObjects, const int32 NewUObjectCount);

	FORCEINLINE void CheckUObjectLimitReached(const int32 NumUObjects, const int32 MaxUObjects, const int32 NewUObjectCount)
	{
		if ((NumUObjects + NewUObjectCount) > MaxUObjects)
		{
			FailMaxUObjectCountExceeded(MaxUObjects, NewUObjectCount);
		}
	}
}

class FChunkedFixedUObjectArray
{
	enum
	{
		NumElementsPerChunk = 64 * 1024,
	};

	/** Primary table to chunks of pointers **/
	std::atomic<FUObjectItem*>* Objects;
	/** If requested, a contiguous memory where all objects are allocated **/
	std::atomic<FUObjectItem*> PreAllocatedObjects;
	/** Maximum number of elements **/
	std::atomic<int32> MaxElements;
	/** Number of elements we currently have **/
	std::atomic<int32> NumElements;
	/** Maximum number of chunks **/
	std::atomic<int32> MaxChunks;
	/** Number of chunks we currently have **/
	std::atomic<int32> NumChunks;

	/**
	* Allocates new chunk for the array
	**/
	void ExpandChunksToIndex(int32 Index) //TSAN_SAFE
	{
		if (Index < 0 || Index > MaxElements)
		{
			//예외 처리
			return;
		}
		int32 ChunkIndex = Index / NumElementsPerChunk;
		while (ChunkIndex >= NumChunks.load(std::memory_order_acquire))
		{
			// add a chunk, and make sure nobody else tries
			std::atomic<::FUObjectItem*>* Chunk = &Objects[NumChunks];
			std::atomic<::FUObjectItem*> NewChunk = new ::FUObjectItem[NumElementsPerChunk];

			::FUObjectItem* expected = nullptr;
			if ((void**)Chunk->compare_exchange_strong(expected, NewChunk, std::memory_order_acq_rel))
			{
				// someone else beat us to the add, we don't support multiple concurrent adds
				assert(false);
			}
			else
			{
				NumChunks++;
				assert(NumChunks <= MaxChunks);
			}
		}
		assert(ChunkIndex < NumChunks && Objects[ChunkIndex]); // should have a valid pointer now
	}

public:

	/** Constructor : Probably not thread safe **/
	FChunkedFixedUObjectArray() //TSAN_SAFE
		: Objects(nullptr)
		, PreAllocatedObjects(nullptr)
		, MaxElements(0)
		, NumElements(0)
		, MaxChunks(0)
		, NumChunks(0)
	{
	}

	~FChunkedFixedUObjectArray()
	{
		if (!PreAllocatedObjects)
		{
			for (int32 ChunkIndex = 0; ChunkIndex < MaxChunks; ++ChunkIndex)
			{
				delete[] Objects[ChunkIndex];
			}
		}
		else
		{
			delete[] PreAllocatedObjects;
		}
		delete[] Objects;
	}

	/**
	* Expands the array so that Element[Index] is allocated. New pointers are all zero.
	* @param Index The Index of an element we want to be sure is allocated
	**/
	void PreAllocate(int32 InMaxElements, bool bPreAllocateChunks) //TSAN_SAFE
	{
		assert(!Objects);
		MaxChunks = InMaxElements / NumElementsPerChunk + 1;
		MaxElements = MaxChunks * NumElementsPerChunk;
		Objects = new std::atomic<::FUObjectItem*>[MaxChunks];
		std::memset(Objects, 0, sizeof(::FUObjectItem*) * MaxChunks);
		if (bPreAllocateChunks)
		{
			// Fully allocate all chunks as contiguous memory
			PreAllocatedObjects = new ::FUObjectItem[MaxElements];
			for (int32 ChunkIndex = 0; ChunkIndex < MaxChunks; ++ChunkIndex)
			{
				Objects[ChunkIndex] = PreAllocatedObjects + ChunkIndex * NumElementsPerChunk;
			}
			NumChunks.store(MaxChunks.load(std::memory_order_relaxed), std::memory_order_relaxed);
		}
	}

	/**
	* Return the number of elements in the array
	* Thread safe, but you know, someone might have added more elements before this even returns
	* @return	the number of elements in the array
	**/
	FORCEINLINE int32 Num() const
	{
		return NumElements;
	}

	/**
	* Return the number max capacity of the array
	* Thread safe, but you know, someone might have added more elements before this even returns
	* @return	the maximum number of elements in the array
	**/
	FORCEINLINE int32 Capacity() const //TSAN_SAFE
	{
		return MaxElements;
	}

	/**
	* Return if this index is valid
	* Thread safe, if it is valid now, it is valid forever. Other threads might be adding during this call.
	* @param	Index	Index to test
	* @return	true, if this is a valid
	**/
	FORCEINLINE bool IsValidIndex(int32 Index) const
	{
		return Index < Num() && Index >= 0;
	}

	/**
	* Return a pointer to the pointer to a given element
	* @param Index The Index of an element we want to retrieve the pointer-to-pointer for
	**/
	FORCEINLINE FUObjectItem const* GetObjectPtr(int32 Index) const //TSAN_SAFE
	{
		const uint32 ChunkIndex = (uint32)Index / NumElementsPerChunk;
		const uint32 WithinChunkIndex = (uint32)Index % NumElementsPerChunk;
		assert(IsValidIndex(Index), TEXT("IsValidIndex(%d)"), Index);
		assert(ChunkIndex < (uint32)NumChunks, TEXT("ChunkIndex (%d) < NumChunks (%d)"), ChunkIndex, NumChunks);
		assert(Index < MaxElements, TEXT("Index (%d) < MaxElements (%d)"), Index, MaxElements);
		FUObjectItem* Chunk = Objects[ChunkIndex];
		assert(Chunk);
		return Chunk + WithinChunkIndex;
	}
	FORCEINLINE FUObjectItem* GetObjectPtr(int32 Index) //TSAN_SAFE
	{
		const uint32 ChunkIndex = (uint32)Index / NumElementsPerChunk;
		const uint32 WithinChunkIndex = (uint32)Index % NumElementsPerChunk;
		assert(IsValidIndex(Index), TEXT("IsValidIndex(%d)"), Index);
		assert(ChunkIndex < (uint32)NumChunks, TEXT("ChunkIndex (%d) < NumChunks (%d)"), ChunkIndex, NumChunks);
		assert(Index < MaxElements, TEXT("Index (%d) < MaxElements (%d)"), Index, MaxElements);
		FUObjectItem* Chunk = Objects[ChunkIndex];
		assert(Chunk);
		return Chunk + WithinChunkIndex;
	}

	FORCEINLINE void PrefetchObjectPtr(int32 Index) const //TSAN_SAFE
	{
		const uint32 ChunkIndex = (uint32)Index / NumElementsPerChunk;
		const uint32 WithinChunkIndex = (uint32)Index % NumElementsPerChunk;
		const FUObjectItem* Chunk = Objects[ChunkIndex];
		_mm_prefetch(reinterpret_cast<const char*>(Chunk + WithinChunkIndex), _MM_HINT_T0);
	}

	/**
	* Return a reference to an element
	* @param	Index	Index to return
	* @return	a reference to the pointer to the element
	* Thread safe, if it is valid now, it is valid forever. This might return nullptr, but by then, some other thread might have made it non-nullptr.
	**/
	FORCEINLINE FUObjectItem const& operator[](int32 Index) const
	{
		FUObjectItem const* ItemPtr = GetObjectPtr(Index);
		assert(ItemPtr);
		return *ItemPtr;
	}
	FORCEINLINE FUObjectItem& operator[](int32 Index)
	{
		FUObjectItem* ItemPtr = GetObjectPtr(Index);
		assert(ItemPtr);
		return *ItemPtr;
	}

	int32 AddRange(int32 NumToAdd)
	{
		int32 Result = NumElements;
		UObjectArrayPrivate::CheckUObjectLimitReached(Result, MaxElements, NumToAdd);
		ExpandChunksToIndex(Result + NumToAdd - 1);
		NumElements += NumToAdd;
		return Result;
	}

	int32 AddSingle()
	{
		return AddRange(1);
	}

	/**
	* Return a naked pointer to the fundamental data structure for debug visualizers.
	**/
	FUObjectItem*** GetRootBlockForDebuggerVisualizers()
	{
		return nullptr;
	}

	int64 GetAllocatedSize() const
	{
		return MaxChunks * sizeof(FUObjectItem*) + NumChunks * NumElementsPerChunk * sizeof(FUObjectItem);
	}
};

class FUObjectArray
{
	friend class UObject;
	friend UObject* StaticAllocatedObject(const UClass*, UObject*, FName, EObjectFlags, EInternalObjectFlags, bool, bool*);

private:
	void ResetSerialNumber(UObject* Object);

public:
	enum ESerialNumberConstants
	{
		START_SERIAL_NUMBER = 1000,
	};


	class FUObjectCreateListener
	{
	public:
		virtual ~FUObjectCreateListener() {}

		/**
		 UObjectBase가 uobject 배열에 추가되었음을 알리는 알림(notification)
		 @param Object: uobject 배열에 추가된 UObjectBase 객체
		 @param Index: 추가된 객체의 배열 내 인덱스
		 */ 
		virtual void NotifyUObjectCreated(const class UObject* Object, int32 Index) = 0;

		/*UObject Array가 종료될 때 호출되며, 여기서 모든 Listner를 제거해야 합니다 */
		virtual void OnUObjectArrayShutdown() = 0;
	};

	class FUObjectDeleteListener
	{
	public:
		virtual ~FUObjectDeleteListener() {}
		/**
		* UObjectBase가 uobject 배열에서 제거되었다는 알림을 제공합니다
		*
		* @param 객체가 파괴되었습니다
		* @param 삭제 중인 객체의 인덱스 인덱스
		*/
		virtual void NotifyUObjectDeleted(const class UObjectBase* Object, int32 Index) = 0;

		/**
		* UObject Array가 종료될 때 호출되며, 여기서 모든 Listner 제거해야 합니다
		*/
		virtual void OnUObjectArrayShutdown() = 0;

		/**
		* 이 Listner가 내부적으로 할당한 힙 메모리의 크기를 반환합니다
		*/
		virtual SIZE_T GetAllocatedSize() const
		{
			return 0;
		}
	};

	FUObjectArray();

	/**
	* 영구 객체 풀을 할당하고 초기화합니다
	*
	* @param MaxUObjects는 배열에 존재할 수 있는 최대 UObjects 수를 나타냅니다
	* @param MaxObjectsNotconcidedByGC 영구 객체 풀에 있는 객체 수
	*/
	void AllcatedObjectPool(int32 MaxObjects, int32 MaxObjectsNotConsideredByGC, bool bPreAllocateArray);
	
	/**
	* GC 최적화에 대한 무시를 비활성화합니다.
	*
	*/
	void DisableDisregardForGC();

	/**
	* disregard Pool에 충분한 여유가 있으면 다시 열고 개체를 계속 추가할 수 있습니다
	*/
	void OpenDisregardForGC();

	/**
	* 초기 로드 후, disregard pool을 닫아서 새 객체가 GC를 사용할 수 있도록 합니다
	*/
	void CloseDisregardForGC();
	

	bool DisregardForGCEnabled() const
	{
		return MaxObjectsNotConsideredByGC > 0;
	}

	void AllocateUObjectIndex(class UObjectBase* Object, EInternalObjectFlags InitialFlags, int32 AlreadyAllocatedIndex = -1, int32 SerialNumber = 0);

	void FreeUObjectIndex(class UObjectBase* Object);

	FORCEINLINE int32 ObjectToIndex(const class UObject* Object) const
	{
		return Object->GetInternalIndex();
	}

	FORCEINLINE FUObjectItem* IndexToObject(int32 Index)
	{
		if (Index < 0) return nullptr;
		if (Index < ObjObjects.Num())
		{
			return const_cast<FUObjectItem*>(&ObjObjects[Index]);
		}
		return nullptr;
	}

	FORCEINLINE FUObjectItem* IndexToObjectUnsafeForGC(int32 Index)
	{
		return const_cast<FUObjectItem*>(&ObjObjects[Index]);
	}

	FORCEINLINE FUObjectItem* IndexToObject(int32 Index, bool bEvenIfGarbage)
	{
		FUObjectItem* ObjectItem = IndexToObject(Index);
		if (ObjectItem && ObjectItem->Object)
		{
			if (!bEvenIfGarbage && ObjectItem->HasAnyFlags(EInternalObjectFlags::Garbage))
			{
				ObjectItem = nullptr;
			}
		}
		return ObjectItem;
	}

	FORCEINLINE bool IsValid(FUObjectItem* ObjectItem, bool bEvenIfGarbage)
	{
		if (ObjectItem)
		{
			return bEvenIfGarbage ? !ObjectItem->IsUnreachable() : !(ObjectItem->HasAnyFlags(EInternalObjectFlags::Unreachable | EInternalObjectFlags::Garbage));
		}
		return false;
	}

	FORCEINLINE FUObjectItem* IndexToValidObject(int32 Index, bool bEvenIfGarbage)
	{
		FUObjectItem* ObjectItem = IndexToObject(Index);
		return IsValid(ObjectItem, bEvenIfGarbage) ? ObjectItem : nullptr;
	}

	FORCEINLINE bool IsValid(int32 Index, bool bEvenIfGarbage)
	{
		// This method assumes Index points to a valid object.
		FUObjectItem* ObjectItem = IndexToObject(Index);
		return IsValid(ObjectItem, bEvenIfGarbage);
	}

	FORCEINLINE bool IsStale(FUObjectItem* ObjectItem, bool bIncludingGarbage)
	{
		// This method assumes ObjectItem is valid.
		return bIncludingGarbage ? (ObjectItem->HasAnyFlags(EInternalObjectFlags::Unreachable | EInternalObjectFlags::Garbage)) : (ObjectItem->IsUnreachable());
	}

	FORCEINLINE bool IsStale(int32 Index, bool bIncludingGarbage)
	{
		// This method assumes Index points to a valid object.
		FUObjectItem* ObjectItem = IndexToObject(Index);
		if (ObjectItem)
		{
			return IsStale(ObjectItem, bIncludingGarbage);
		}
		return true;
	}

	/** Returns the index of the first object outside of the disregard for GC pool */
	FORCEINLINE int32 GetFirstGCIndex() const
	{
		return ObjFirstGCIndex;
	}

	/**
	 * Adds a new listener for object creation
	 *
	 * @param Listener listener to notify when an object is deleted
	 */
	void AddUObjectCreateListener(FUObjectCreateListener* Listener);

	/**
	 * Removes a listener for object creation
	 *
	 * @param Listener listener to remove
	 */
	void RemoveUObjectCreateListener(FUObjectCreateListener* Listener);

	/**
	 * Adds a new listener for object deletion
	 *
	 * @param Listener listener to notify when an object is deleted
	 */
	void AddUObjectDeleteListener(FUObjectDeleteListener* Listener);

	/**
	 * Removes a listener for object deletion
	 *
	 * @param Listener listener to remove
	 */
	void RemoveUObjectDeleteListener(FUObjectDeleteListener* Listener);

	/**
	 * Removes an object from delete listeners
	 *
	 * @param Object to remove from delete listeners
	 */
	void RemoveObjectFromDeleteListeners(UObjectBase* Object);

	/**
	 * Checks if a UObject pointer is valid
	 *
	 * @param	Object object to test for validity
	 * @return	true if this index is valid
	 */
	bool IsValid(const UObjectBase* Object) const;

	/** Checks if the object index is valid. */
	FORCEINLINE bool IsValidIndex(const UObject* Object) const
	{
		return ObjObjects.IsValidIndex(Object->GetInternalIndex());
	}

	/**
	 * Returns true if this object is "disregard for GC"...same results as the legacy RF_DisregardForGC flag
	 *
	 * @param Object object to get for disregard for GC
	 * @return true if this object si disregard for GC
	 */
	FORCEINLINE bool IsDisregardForGC(const class UObject* Object)
	{
		return (Object->GetInternalIndex() <= ObjLastNonGCIndex);
	}
	/**
	 * Returns the size of the global UObject array, some of these might be unused
	 *
	 * @return	the number of UObjects in the global array
	 */
	FORCEINLINE int32 GetObjectArrayNum() const
	{
		return ObjObjects.Num();
	}

	/**
	 * Returns the size of the global UObject array minus the number of permanent objects
	 *
	 * @return	the number of UObjects in the global array
	 */
	FORCEINLINE int32 GetObjectArrayNumMinusPermanent() const
	{
		return ObjObjects.Num() - (ObjLastNonGCIndex + 1);
	}

	/**
	 * Returns the number of permanent objects
	 *
	 * @return	the number of permanent objects
	 */
	FORCEINLINE int32 GetObjectArrayNumPermanent() const
	{
		return ObjLastNonGCIndex + 1;
	}

	/**
	 * Returns the number of actual object indices that are claimed (the total size of the global object array minus
	 * the number of available object array elements
	 *
	 * @return	The number of objects claimed
	 */
	int32 GetObjectArrayNumMinusAvailable() const
	{
		return ObjObjects.Num() - ObjAvailableList.Num();
	}

	/**
	* Returns the estimated number of object indices available for allocation
	*/
	int32 GetObjectArrayEstimatedAvailable() const
	{
		return ObjObjects.Capacity() - GetObjectArrayNumMinusAvailable();
	}

	/**
	* Returns the estimated number of object indices available for allocation
	*/
	int32 GetObjectArrayCapacity() const
	{
		return ObjObjects.Capacity();
	}

	/**
	 * Clears some internal arrays to get rid of false memory leaks
	 */
	void ShutdownUObjectArray();

	/**
	* Given a UObject index return the serial number. If it doesn't have a serial number, give it one. Threadsafe.
	* @param Index - UObject Index
	* @return - the serial number for this UObject
	*/
	int32 AllocateSerialNumber(int32 Index);

	/**
	* Given a UObject index return the serial number. If it doesn't have a serial number, return 0. Threadsafe.
	* @param Index - UObject Index
	* @return - the serial number for this UObject
	*/
	FORCEINLINE int32 GetSerialNumber(int32 Index)
	{
		FUObjectItem* ObjectItem = IndexToObject(Index);
		if (ObjectItem)
		{
			return ObjectItem->GetSerialNumber();
		}
	}

	// TODO : 나중에 MultiThread로 되면 해야됨
//	/** Locks the internal object array mutex */
//	void LockInternalArray() const
//	{
//#if THREADSAFE_UOBJECTS
//		ObjObjectsCritical.Lock();
//#else
//		check(IsInGameThread());
//#endif
//	}
//
//	/** Unlocks the internal object array mutex */
//	void UnlockInternalArray() const
//	{
//#if THREADSAFE_UOBJECTS
//		ObjObjectsCritical.Unlock();
//#endif
//	}

	class TIterator
	{
	public:
		enum EEndTagType
		{
			EndTag
		};

		/**
		 * Constructor
		 *
		 * @param	InArray				the array to iterate on
		 * @param	bOnlyGCedObjects	if true, skip all of the permanent objects
		 */
		TIterator(const FUObjectArray& InArray, bool bOnlyGCedObjects = false) :
			Array(InArray),
			Index(-1),
			CurrentObject(nullptr)
		{
			if (bOnlyGCedObjects)
			{
				Index = Array.ObjLastNonGCIndex;
			}
			Advance();
		}

		/**
		 * Constructor
		 *
		 * @param	InArray				the array to iterate on
		 * @param	bOnlyGCedObjects	if true, skip all of the permanent objects
		 */
		TIterator(EEndTagType, const TIterator& InIter) :
			Array(InIter.Array),
			Index(Array.ObjObjects.Num())
		{
		}

		/**
		 * Iterator advance
		 */
		FORCEINLINE void operator++()
		{
			Advance();
		}

		bool operator==(const TIterator& Rhs) const { return Index == Rhs.Index; }
		bool operator!=(const TIterator& Rhs) const { return Index != Rhs.Index; }

		/** Conversion to "bool" returning true if the iterator is valid. */
		FORCEINLINE explicit operator bool() const
		{
			return !!CurrentObject;
		}
		/** inverse of the "bool" operator */
		FORCEINLINE bool operator !() const
		{
			return !(bool)*this;
		}

		FORCEINLINE int32 GetIndex() const
		{
			return Index;
		}

	protected:

		/**
		 * Dereferences the iterator with an ordinary name for clarity in derived classes
		 *
		 * @return	the UObject at the iterator
		 */
		FORCEINLINE FUObjectItem* GetUObject() const
		{
			return CurrentObject;
		}
		/**
		 * Iterator advance with ordinary name for clarity in subclasses
		 * @return	true if the iterator points to a valid object, false if iteration is complete
		 */
		FORCEINLINE bool Advance()
		{
			//@todo UE check this for LHS on Index on consoles
			FUObjectItem* NextObject = nullptr;
			CurrentObject = nullptr;
			while (++Index < Array.GetObjectArrayNum())
			{
				NextObject = const_cast<FUObjectItem*>(&Array.ObjObjects[Index]);
				if (NextObject->Object)
				{
					CurrentObject = NextObject;
					return true;
				}
			}
			return false;
		}

		/** Gets the array this iterator iterates over */
		const FUObjectArray& GetIteratedArray() const
		{
			return Array;
		}

	private:
		/** the array that we are iterating on, probably always GUObjectArray */
		const FUObjectArray& Array;
		/** index of the current element in the object array */
		int32 Index;
		/** Current object */
		mutable FUObjectItem* CurrentObject;
	};

private:

	//typedef TStaticIndirectArrayThreadSafeRead<UObjectBase, 8 * 1024 * 1024 /* Max 8M UObjects */, 16384 /* allocated in 64K/128K chunks */ > TUObjectArray;
	typedef FChunkedFixedUObjectArray TUObjectArray;


	// note these variables are left with the Obj prefix so they can be related to the historical GObj versions

	/** First index into objects array taken into account for GC.							*/
	int32 ObjFirstGCIndex;
	/** Index pointing to last object created in range disregarded for GC.					*/
	int32 ObjLastNonGCIndex;
	/** Maximum number of objects in the disregard for GC Pool */
	int32 MaxObjectsNotConsideredByGC;

	/** If true this is the intial load and we should load objects int the disregarded for GC range.	*/
	bool OpenForDisregardForGC;
	/** Array of all live objects.											*/
	TUObjectArray ObjObjects;

	/** Available object indices.											*/
	TArray<int32> ObjAvailableList;

	/**
	* Array of things to notify when a UObjectBase is created
	*/
	TArray<FUObjectCreateListener*> UObjectCreateListeners;
	/**
		* Array of things to notify when a UObjectBase is destroyed
		*/
	TArray<FUObjectDeleteListener*> UObjectDeleteListeners;

	/** If set to false object indices won't be recycled to the global pool and can be explicitly reused when creating new objects */
	bool bShouldRecycleObjectIndices = true;


public:
	/** INTERNAL USE ONLY: gets the internal FUObjectItem array */
	TUObjectArray& GetObjectItemArrayUnsafe()
	{
		return ObjObjects;
	}

	const TUObjectArray& GetObjectItemArrayUnsafe() const
	{
		return ObjObjects;
	}

	SIZE_T GetAllocatedSize() const
	{
		return ObjObjects.GetAllocatedSize() + ObjAvailableList.Len() * sizeof(int32) + UObjectCreateListeners.Len() *
			sizeof(FUObjectCreateListener*) + UObjectDeleteListeners.Len() * sizeof(FUObjectDeleteListener*);
	}

	SIZE_T GetDeleteListenersAllocatedSize(int32* OutNumListeners = nullptr) const
	{
#if THREADSAFE_UOBJECTS
		FTransactionallySafeScopeLock ListenersLock(&UObjectDeleteListenersCritical);
#endif
		SIZE_T AllocatedSize = 0;
		for (FUObjectDeleteListener* Listener : UObjectDeleteListeners)
		{
			AllocatedSize += Listener->GetAllocatedSize();
		}
		if (OutNumListeners)
		{
			*OutNumListeners = UObjectDeleteListeners.Num();
		}
		return AllocatedSize;
	}

	void DumpUObjectCountsToLog() const;
};

/** UObject cluster. Groups UObjects into a single unit for GC. */
struct FUObjectCluster
{
	FUObjectCluster()
		: RootIndex(INDEX_NONE)
		, bNeedsDissolving(false)
	{}

	/** Root object index */
	int32 RootIndex;
	/** Objects that belong to this cluster */
	TArray<int32> Objects;
	/** Other clusters referenced by this cluster */
	TArray<int32> ReferencedClusters;
	/** Objects that could not be added to the cluster but still need to be referenced by it */
	TArray<int32> MutableObjects;
	/** List of clusters that direcly reference this cluster. Used when dissolving a cluster. */
	TArray<int32> ReferencedByClusters;
#if WITH_VERSE_VM || defined(__INTELLISENSE__)
	/** All verse cells are considered mutable.  They will just be added directly to verse gc when the cluster is marked */
	TArray<Verse::VCell*> MutableCells;
#endif

	/** Cluster needs dissolving, probably due to PendingKill reference */
	bool bNeedsDissolving;
};

class FUObjectClusterContainer
{
	/** List of all clusters */
	TArray<FUObjectCluster> Clusters;
	/** List of available cluster indices */
	TArray<int32> FreeClusterIndices;
	/** Number of allocated clusters */
	int32 NumAllocatedClusters;
	/** Clusters need dissolving, probably due to PendingKill reference */
	bool bClustersNeedDissolving;

	/** Dissolves a cluster */
	void DissolveCluster(FUObjectCluster& Cluster);

public:

	FUObjectClusterContainer();

	FORCEINLINE FUObjectCluster& operator[](int32 Index)
	{
		static_assert(Index < 0 || Index >= Clusters.Num());
		return Clusters[Index];
	}

	/** Returns an index to a new cluster */
	//int32 AllocateCluster(int32 InRootObjectIndex);

	/** Frees the cluster at the specified index */
	void FreeCluster(int32 InClusterIndex);

	/**
	* Gets the cluster the specified object is a root of or belongs to.
	* @Param ClusterRootOrObjectFromCluster Root cluster object or object that belongs to a cluster
	*/
	FUObjectCluster* GetObjectCluster(UObject* ClusterRootOrObjectFromCluster);


	/** 
	 * Dissolves a cluster and all clusters that reference it 
	 * @Param ClusterRootOrObjectFromCluster Root cluster object or object that belongs to a cluster
	 */
	void DissolveCluster(UObject* ClusterRootOrObjectFromCluster);

	/** 
	 * Dissolve all clusters marked for dissolving 
	 * @param bForceDissolveAllClusters if true, dissolves all clusters even if they're not marked for dissolving
	 */
	void DissolveClusters(bool bForceDissolveAllClusters = false);

	/** Dissolve the specified cluster and all clusters that reference it */
	void DissolveClusterAndMarkObjectsAsUnreachable(FUObjectItem* RootObjectItem);

	/*** Returns the minimum cluster size as specified in ini settings */
	int32 GetMinClusterSize() const;

	/** Gets the clusters array (for internal use only!) */
	TArray<FUObjectCluster>& GetClustersUnsafe() 
	{ 
		return Clusters;  
	}

	/** Returns the number of currently allocated clusters */
	int32 GetNumAllocatedClusters() const
	{
		return NumAllocatedClusters;
	}

	/** Lets the FUObjectClusterContainer know some clusters need dissolving */
	void SetClustersNeedDissolving()
	{
		bClustersNeedDissolving = true;
	}
	
	/** Checks if any clusters need dissolving */
	bool ClustersNeedDissolving() const
	{
		return bClustersNeedDissolving;
	}
};

/** Global UObject allocator							*/
extern FUObjectArray GUObjectArray;
extern FUObjectClusterContainer GUObjectClusters;

