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
{
}

void FUObjectArray::AllcatedObjectPool(int32 MaxObjects, int32 MaxObjectsNotConsideredByGC, bool bPreAllocateArray)
{
}

void FUObjectArray::DisableDisregardForGC()
{
}

void FUObjectArray::OpenDisregardForGC()
{
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
