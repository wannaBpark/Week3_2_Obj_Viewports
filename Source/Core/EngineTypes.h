#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core//HAL/EnumClassFlags.h"

namespace EEndPlayReason
{
enum Type : uint8
{
    /** 명시적인 삭제가 일어났을 때, Destroy()등 */
    Destroyed,
    /** World가 바뀌었을 때 */
    WorldTransition,
    /** 프로그램을 종료했을 때 */
    Quit,
};
}

enum EObjectFlags
{
	RF_NoFlags = 0x00000000,

	RF_Public = 0x00000001,	// 객체가 해당 패키지 외부에서도 보일 수 있음을 나타냅니다.
    RF_Standalone = 0x00000002, //객체가 참조되지 않더라도 편집을 위해 유지되어야 함을 나타냅니다.
    RF_MarkAsNative = 0x00000004, // 생성 시 객체(특히 UField 계열)가 네이티브로 표시됨을 나타내며, HasAnyFlags() 같은 함수에서는 사용하지 말아야 합니다.
    RF_ClassDefaultObject = 0x00000010, //해당 클래스의 기본 템플릿으로 사용되는 객체임을 나타냅니다. 각 클래스마다 하나씩 생성됩니다.
	RF_ArchetypeObject = 0x00000020, //객체 인스턴스를 생성할 때 템플릿으로 사용할 수 있는 객체임을 나타냅니다.
	RF_Transient = 0x00000040, //객체가 저장되지 않아야 함을 나타냅니다.

    RF_MarkAsRootSet = 0x00000080, // 객체가 생성될 때 루트 집합으로 표시되어, 참조되지 않더라도 가비지 컬렉션에서 제거되지 않음을 보장합니다. (HasAnyFlags()에서는 사용하지 말 것)
    RF_TagGarbageTemp = 0x00000100, // 임시 사용자 플래그로, 가비지 컬렉션과 관련된 유틸리티에서 사용됩니다. 가비지 컬렉터 자체는 이 플래그를 해석하지 않습니다.

    RF_MirroredGarbage = 0x40000000, //논리적으로 가비지로 간주되어 참조하면 안 되는 객체임을 나타냅니다. 이 플래그는 내부적으로 가비지 컬렉터의 성능 최적화를 위해 EInternalObjectFlags의 Garbage 플래그와 미러링됩니다.

    RF_AllocatedInSharedPage = 0x80000000, // 여러 UObject가 공유하는 참조 카운트 페이지에서 할당되었음을 나타냅니다.
};

ENUM_CLASS_FLAGS(EObjectFlags);

enum class EInternalObjectFlags : int32
{
    None = 0,
    ReachabilityFlag0 = 1 << 0,
    ReachabilityFlag1 = 1 << 1,
    ReachabilityFlag2 = 1 << 2,

    LoaderImport = 1 << 20,
    Garbage = 1 << 21,
    AsyncLoadingPhase1 = 1 << 22, ///< Object is being asynchronously loaded.
    ReachableInCluster = 1 << 23, ///< External reference to object in cluster exists
    ClusterRoot = 1 << 24,
    Native = 1 << 25, ///< Native (UClass only). 
    Async = 1 << 26, ///< Object exists only on a different thread than the game thread.
    AsyncLoadingPhase2 = 1 << 27, ///< Object is being asynchronously loaded.
    Unreachable = 1 << 28,
    RefCounted = 1 << 29,
	RootSet = 1 << 30,
	PendingConstruction = 1 << 31,
};
ENUM_CLASS_FLAGS(EInternalObjectFlags);

#define EInternalObjectFlags_GarbageCollectionKeepFlags (EInternalObjectFlags::Native | EInternalObjectFlags::Async | EInternalObjectFlags::AsyncLoadingPhase1 | EInternalObjectFlags::AsyncLoadingPhase2 | EInternalObjectFlags::LoaderImport | EInternalObjectFlags::RefCounted)
#define EInternalObjectFlags_AllFlags (EInternalObjectFlags::ReachabilityFlag0 | EInternalObjectFlags::ReachabilityFlag1 | EInternalObjectFlags::ReachabilityFlag2 | EInternalObjectFlags::LoaderImport | EInternalObjectFlags::Garbage | EInternalObjectFlags::ReachableInCluster | EInternalObjectFlags::ClusterRoot | EInternalObjectFlags::Native | EInternalObjectFlags::RefCounted | EInternalObjectFlags::Async | EInternalObjectFlags::AsyncLoadingPhase1 | EInternalObjectFlags::AsyncLoadingPhase2 | EInternalObjectFlags::RootSet | EInternalObjectFlags::PendingConstruction | (EInternalObjectFlags::Unreachable))
#define EInternalObjectFlags_RootFlags (EInternalObjectFlags::RootSet | EInternalObjectFlags_GarbageCollectionKeepFlags)
#define EInternalObjectFlags_ReachabilityFlags (EInternalObjectFlags::ReachabilityFlag0 | EInternalObjectFlags::ReachabilityFlag1 | EInternalObjectFlags::ReachabilityFlag2 | EInternalObjectFlags::Unreachable)
#define EInternalObjectFlags_AsyncLoading (EInternalObjectFlags::AsyncLoadingPhase1 | EInternalObjectFlags::AsyncLoadingPhase2)




