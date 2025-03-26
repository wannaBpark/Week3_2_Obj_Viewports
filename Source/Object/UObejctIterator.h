#pragma once
#include "Core/EngineTypes.h"
#include "Core/Container/Array.h"
#include "Core/HAL/PlatformType.h"
#include "Core/Container/ObjectHash.h"

enum class EObjectIteratorThreadSafetyOptions : uint8
{
	None = 0,
	ThreadSafe = 1, // Use only with function local iterators. Persistent iterators may prevent (hang) UObject creation on worker threads otherwise.
	ThreadSafeAdvance = 2 // Can be used for global iterators but locks the global UObjectArray each time the iterator advances to the next object (can be slow but does not result in deadlocks).
};

inline EInternalObjectFlags GetObjectIteratorDefaultInternalExclusionFlags(EInternalObjectFlags InternalExclusionFlags)
{
	return InternalExclusionFlags | EInternalObjectFlags::Unreachable | EInternalObjectFlags::PendingConstruction | EInternalObjectFlags_AsyncLoading;
}

class UObject;
/*UE5에서 UObjectIterator는 메모리에 존재하는 모든 UObject 인스턴스를 순회(iterate)할 수 있도록 제공되는 도구입니다. 
이 이터레이터를 사용하면 엔진이나 개발자가 전체 객체 리스트를 순회하면서 객체를 검사하거나, 디버깅, 또는 GC(가비지 컬렉션)와 관련된 작업을 수행할 수 있습니다.*/
template<class T>
class TObjectIterator
{
public:
    enum EEndTagType
    {
        EndTag,
    };

    // 생성자: 추가 제외 플래그와 파생 클래스 포함 여부에 따라 T::StaticClass()의 객체들을 ObjectArray에 채우고, 이터레이터를 초기화합니다.
    explicit TObjectIterator(EObjectFlags AdditionalExclusionFlags = RF_ClassDefaultObject, bool bIncludeDerivedClasses = true, EInternalObjectFlags InInternalExclusionFlags = EInternalObjectFlags::None)
        : Index(-1)
    {
        GetObjectsOfClass(T::StaticClass(), ObjectArray, bIncludeDerivedClasses, AdditionalExclusionFlags, GetObjectIteratorDefaultInternalExclusionFlags(InInternalExclusionFlags));
        Advance();
    }

    /**
    * 생성자
    */
    TObjectIterator(EEndTagType, const TObjectIterator& Begin)
        : Index(Begin.ObjectArray.Num())
    {
    }

    /**
     * 이터레이터 진행 (다음 요소로 이동)
     */
    inline void operator++()
    {
        Advance();
    }

    /** 
     * 이터레이터가 유효하면 true를 반환하는 bool 형으로의 변환.
     */
    inline explicit operator bool() const
    {
        if (Index < 0 || Index >= ObjectArray.Num())
            return false;
        return true;
    }
    /** 
     * "bool" 연산자의 반대. 
     */
    inline bool operator !() const 
    {
        return !static_cast<bool>(*this);
    }

    /**
     * 이터레이터 역참조
     * @return 이터레이터가 가리키는 객체 포인터를 반환합니다.
     */
    inline T* operator*() const { return static_cast<T*>(GetUObject()); }

    /**
     * 이터레이터 역참조
     * @return 이터레이터가 가리키는 객체 포인터를 반환합니다.
     */
    inline T* operator->() const { return static_cast<T*>(GetUObject()); }

    inline bool operator==(const TObjectIterator& Rhs) const { return Index == Rhs.Index; }
    inline bool operator!=(const TObjectIterator& Rhs) const { return Index != Rhs.Index; }

protected:
    /**
     * 하위 클래스에서 명확하게 사용하기 위해, 이터레이터를 역참조합니다.
     *
     * @return 이터레이터가 가리키는 UObject를 반환합니다.
     */
    inline UObject* GetUObject() const 
    { 
        return ObjectArray[Index];
    }
    
    /**
     * 하위 클래스에서 명확하게 사용하기 위해 이터레이터를 진행시킵니다.
     *
     * @return 이터레이터가 유효한 객체를 가리키면 true, 순회가 완료되었으면 false를 반환합니다.
     */
    FORCEINLINE bool Advance()
    {
        //@todo UE: 콘솔에서 Index의 좌변 검사를 확인할 것
        while (++Index < ObjectArray.Num())
        {
            if (GetUObject())
            {
                return true;
            }
        }
        return false;
    }

protected:
    /** GetObjectsOfClass 쿼리의 결과로 얻은 객체 배열 */
    TArray<UObject*> ObjectArray;
    /** 객체 배열 내 현재 요소의 인덱스 */
    int32 Index;
};

template <typename T>
struct TObjectRange
{
	TObjectRange(EObjectFlags AdditionalExclusionFlags = RF_ClassDefaultObject, bool bIncludeDerivedClasses = true, EInternalObjectFlags InInternalExclusionFlags = EInternalObjectFlags::None)
	: Begin(AdditionalExclusionFlags, bIncludeDerivedClasses, InInternalExclusionFlags)
	{
	}

	friend TObjectIterator<T> begin(const TObjectRange& Range) { return Range.Begin; }
	friend TObjectIterator<T> end  (const TObjectRange& Range) { return TObjectIterator<T>(TObjectIterator<T>::EndTag, Range.Begin); }

	TObjectIterator<T> Begin;
};
