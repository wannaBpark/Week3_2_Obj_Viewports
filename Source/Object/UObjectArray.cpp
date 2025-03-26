#include "UObjectArray.h"
#include <Debug/DebugConsole.h>

void FUObjectItem::SetMaybeUnreachable()
{
    // 나중에 GCFlags에서 뭔가 해야되는 거 같음
    // UE::GC::Private::FGCFlags::SetMaybeUnreachable_ForGC(this);
	// 밑에게 일단 캡슐화 안 한거
	AtomicallySetFlag_ForGC(EInternalObjectFlags::Unreachable);
}

bool FUObjectItem::IsMaybeUnreachable() const
{    
    // 나중에 GCFlags에서 뭔가 해야되는 거 같음
    // return UE::GC::Private::FGCFlags::IsMaybeUnreachable_ForGC(this);
    // 밑에게 일단 캡슐화 안 한거
	return !!(GetFlagsInternal() & int32(EInternalObjectFlags::Unreachable));
}

void FUObjectItem::FastMarkAsReachableInterlocked_ForGC()
{
	std::atomic_fetch_and(&Flags, ~int32(EInternalObjectFlags::ReachabilityFlag1));
	std::atomic_fetch_or(&Flags, int32(EInternalObjectFlags::ReachabilityFlag0));
}

void FUObjectItem::FastMarkAsReachableAndClearReachaleInClusterInterlocked_ForGC()
{
	// 나중에 GCFlags에서 뭔가 해야되는 거 같음
    // UE::GC::Private::FGCFlags::FastMarkAsReachableAndClearReachableInClusterInterlocked_ForGC(this);
	// 밑에게 일단 캡슐화 안 한거
	std::atomic_fetch_and(&Flags, ~int32(EInternalObjectFlags::ReachabilityFlag1 | EInternalObjectFlags::ReachableInCluster));
	std::atomic_fetch_or(&Flags, int32(EInternalObjectFlags::ReachabilityFlag0));
}

bool FUObjectItem::MarkAsReachableInterlocked_ForGC()
{
	// 나중에 GCFlags에서 뭔가 해야되는 거 같음
    //	UE::GC::Private::FGCFlags::FastMarkAsReachableInterlocked_ForGC(this);
	// 밑에게 일단 캡슐화 안 한거
	const int32 FlagToClear = static_cast<int32>(EInternalObjectFlags::ReachabilityFlag1);
    if (Flags.load(std::memory_order_relaxed) & FlagToClear)
    {
        // 플래그를 제거하면서 이전 값을 반환 (acq_rel 순서)
        int32_t Old = Flags.fetch_and(~FlagToClear, std::memory_order_acq_rel);
        // 추가적으로 ReachableObjectFlag를 설정 (acq_rel 순서)
        Flags.fetch_or(static_cast<int32_t>(EInternalObjectFlags::ReachabilityFlag0), std::memory_order_acq_rel);
        return (Old & FlagToClear);
    }
    return false;
}

bool FUObjectItem::SetRootFlags(EInternalObjectFlags FlagsToSet)
{
    constexpr int32 RootFlags = static_cast<int32>(EInternalObjectFlags_RootFlags);
	bool bIChangedIt = false;
    {
        const int32 OldFlags = Flags.load(std::memory_order_relaxed);
        if ((OldFlags & RootFlags) == 0)
        {
            // TODO : Lock
            //if (!(GUObjectArray.IsOpenForDisregardForGC() & GUObjectArray.DisregardForGCEnabled()))
            //{
            //    GRoots.Add(GUObjectArray.ObjectToIndex(Object));
            //}
        }
        // atomic fetch_or를 사용하여 FlagsToSet에 해당하는 비트를 원자적으로 설정합니다.
        // fetch_or는 이전 플래그 값을 반환하므로, (~PreviousFlags)와 FlagsToSet의 교집합으로
        // 새로 설정된 비트들을 계산할 수 있습니다.
        bIChangedIt = AtomicallySetFlag_ForGC(FlagsToSet);
    }
	if (bIChangedIt)
	{
        //Object->MarkAsReachable();
	}
    return bIChangedIt;
}

bool FUObjectItem::ClearRootFlags(EInternalObjectFlags FlagsToClear)
{
    constexpr int32 RootFlags = (int32)EInternalObjectFlags_RootFlags;
    // TODO : Lock
    const int32 OldFlags = GetFlagsInternal();

    // Now that we have the lock, we prevent the removal if refcount is not 0
    // This avoids a race condition between ReleaseRef and AddRef.
    if ((FlagsToClear & EInternalObjectFlags::RefCounted) == EInternalObjectFlags::RefCounted && GetRefCountInternal())
    {
        FlagsToClear &= ~EInternalObjectFlags::RefCounted;
    }

    if ((OldFlags & RootFlags) != 0 && ((OldFlags & ~(int32)FlagsToClear) & RootFlags) == 0)
    {
        //GRoots.Remove(GUObjectArray.ObjectToIndex(Object));
    }
    return AtomicallyClearFlag_ForGC((EInternalObjectFlags)FlagsToClear);
}

static int32 GVarDumpObjectCountsToLogWhenMaxObjectLimitExceeded = 0;

void UObjectArrayPrivate::FailMaxUObjectCountExceeded(const int32 MaxUObjects, const int32 NewUObjectCount)
{
    if (GVarDumpObjectCountsToLogWhenMaxObjectLimitExceeded)
    {
       // GUObjectArray.DumpUObjectCountsToLog();
    }
    UE_LOG(TEXT("Maximum number of UObjects (%d) exceeded when trying to add %d object(s), make sure you update MaxObjectsInGame/MaxObjectsInEditor/MaxObjectsInProgram in project settings."), MaxUObjects, NewUObjectCount);
}

UObject* StaticAllocatedObject(const UClass* InClass, UObject* InOuter, FName InName, EObjectFlags InFlags, EInternalObjectFlags InternalSetFlags, bool bCanRecycleSubobjects, bool* bOutRecycledSubobject)
{
    // TODO : LLM 메모리 추적
	__assume(InOuter != (UObject*)-1);
	assert(InClass);

	const bool bCreatingCDO = (InFlags & RF_ClassDefaultObject) != 0;
	const bool bCreatingArchetype = (InFlags & RF_ArchetypeObject) != 0;

    /*if (StaticAllocateObjectErrorTests(InClass, InOuter, InName, InFlags))
    {
        return nullptr;
    }*/



    return nullptr;
}

void FUObjectArray::ResetSerialNumber(UObject* Object)
{
}

FUObjectArray::FUObjectArray()
: ObjFirstGCIndex(0)
, ObjLastNonGCIndex(INDEX_NONE)
, MaxObjectsNotConsideredByGC(0)
, OpenForDisregardForGC(true)
//, PrimarySerialNumber(START_SERIAL_NUMBER)
{
}

void FUObjectArray::AllcatedObjectPool(int32 InMaxUObjects, int32 InMaxObjectsNotConsideredByGC, bool bPreAllocateObjectArray)
{
    MaxObjectsNotConsideredByGC = InMaxObjectsNotConsideredByGC;

    // GObjFirstGCIndex is the index at which the garbage collector will start for the mark phase.
    // If disregard for GC is enabled this will be set to an invalid value so that later we
    // know if disregard for GC pool has already been closed (at least once)
    ObjFirstGCIndex = DisregardForGCEnabled() ? -1 : 0;

    // Pre-size array.
    static_assert(ObjObjects.Num() != 0);
    if (InMaxUObjects <= 0)
    {
        UE_LOG(TEXT("Max UObject count is invalid. It must be a number that is greater than 0."));
    }
    ObjObjects.PreAllocate(InMaxUObjects, bPreAllocateObjectArray);

    if (MaxObjectsNotConsideredByGC > 0)
    {
        ObjObjects.AddRange(MaxObjectsNotConsideredByGC);
    }
}

void FUObjectArray::DisableDisregardForGC()
{
    OpenForDisregardForGC = true;
}

void FUObjectArray::OpenDisregardForGC()
{
    static_assert(OpenForDisregardForGC == false);

    // // Make sure all classes that have been loaded/created so far are properly initialized
    // if (!IsEngineExitRequested())
    // {
    //     ProcessNewlyLoadedUObjects();
    //
    //     UClass::AssembleReferenceTokenStreams();
    //
    //     if (GIsInitialLoad)
    //     {
    //         // Iterate over all objects and mark them to be part of root set.
    //         int32 NumAlwaysLoadedObjects = 0;
    //         int32 NumRootObjects = 0;
    //         for (FThreadSafeObjectIterator It; It; ++It)
    //         {
    //             UObject* Object = *It;
    //             if (Object->IsSafeForRootSet())
    //             {
    //                 NumRootObjects++;
    //                 Object->AddToRoot();
    //             }
    //             else if (Object->IsRooted())
    //             {
    //                 Object->RemoveFromRoot();
    //             }
    //             NumAlwaysLoadedObjects++;
    //         }
    //
    //         UE_LOG(LogUObjectArray, Log, TEXT("%i objects as part of root set at end of initial load."), NumAlwaysLoadedObjects);
    //         if (GUObjectArray.DisregardForGCEnabled())
    //         {
    //             UE_LOG(LogUObjectArray, Log, TEXT("%i objects are not in the root set, but can never be destroyed because they are in the DisregardForGC set."), NumAlwaysLoadedObjects - NumRootObjects);
    //         }
    //
    //         GUObjectAllocator.BootMessage();
    //     }
    // }
    //
    // // When disregard for GC pool is closed, make sure the first GC index is set after the last non-GC index.
    // // We do allow here for some slack if MaxObjectsNotConsideredByGC > (ObjLastNonGCIndex + 1) so that disregard for GC pool
    // // can be re-opened later.
    // ObjFirstGCIndex = FMath::Max(ObjFirstGCIndex, ObjLastNonGCIndex + 1);
    //
    // UE_LOG(LogUObjectArray, Log, TEXT("CloseDisregardForGC: %d/%d objects in disregard for GC pool"), ObjLastNonGCIndex + 1, MaxObjectsNotConsideredByGC);	
    //
    // OpenForDisregardForGC = false;
    // GIsInitialLoad = false;
    //
    // checkf(!DisregardForGCEnabled() || !GIsEditor, TEXT("Disregard For GC Set can't be enabled when running the editor"));
    // checkf(DisregardForGCEnabled() || (ObjFirstGCIndex == 0 && ObjLastNonGCIndex == -1), TEXT("Disregard for GC Set is not properly disabled (FirstGCIndex = %d, LastNonGCIndex = %d"), ObjFirstGCIndex, ObjLastNonGCIndex);
}

void FUObjectArray::CloseDisregardForGC()
{
}

void FUObjectArray::AllocateUObjectIndex(UObjectBase* Object, EInternalObjectFlags InitialFlags, int32 AlreadyAllocatedIndex, int32 SerialNumber)
{
}

void FUObjectArray::FreeUObjectIndex(UObjectBase* Object)
{
}

void FUObjectArray::AddUObjectCreateListener(FUObjectCreateListener* Listener)
{
}

void FUObjectArray::RemoveUObjectCreateListener(FUObjectCreateListener* Listener)
{
}

void FUObjectArray::AddUObjectDeleteListener(FUObjectDeleteListener* Listener)
{
}

void FUObjectArray::RemoveUObjectDeleteListener(FUObjectDeleteListener* Listener)
{

}

void FUObjectArray::RemoveObjectFromDeleteListeners(UObjectBase* Object)
{
    // int32 Index = Object->InternalIndex;
    // Iterate in reverse order so that when one of the listeners removes itself from the array inside of NotifyUObjectDeleted we don't skip the next listener.
    // for (int32 ListenerIndex = UObjectDeleteListeners.Num() - 1; ListenerIndex >= 0; --ListenerIndex)
    // {
    //     UObjectDeleteListeners[ListenerIndex]->NotifyUObjectDeleted(Object, Index);
    // }
}

bool FUObjectArray::IsValid(const UObjectBase* Object) const
{
    return false;
}

void FUObjectArray::ShutdownUObjectArray()
{
}

int32 FUObjectArray::AllocateSerialNumber(int32 Index)
{
    return int32();
}

void FUObjectArray::DumpUObjectCountsToLog() const
{
}
