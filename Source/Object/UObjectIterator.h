#pragma once
#include "Object/UObject.h"
#include "Core/Container/ObjectHash.h"
#include "Core/Container/Array.h"
#include "Core/Engine.h"

/**
 * 특정 타입의 UObject 인스턴스를 순회하기 위한 반복자 클래스입니다.
 *
 * @tparam T 순회할 UObject 타입 또는 그 파생 클래스
 */
template <typename T>
class TObjectIterator
{
public:
    enum EEndTagType : uint8
    {
        EndTag
    };

    /** Begin 생성자 */
    explicit TObjectIterator(bool bIncludeDerivedClasses = true)
        : Index(-1)
    {
        GetObjectsOfClass(T::StaticClass(), ObjectArray, bIncludeDerivedClasses);
        Advance();
    }

    /** End 생성자 */
    TObjectIterator(EEndTagType, const TObjectIterator& Begin)
        : Index(Begin.ObjectArray.Num())
    {
    }

    FORCEINLINE void operator++()
    {
        Advance();
    }

    FORCEINLINE T* operator* () const
    {
        return (T*)GetUObject();
    }

    FORCEINLINE T* operator-> () const
    {
        return (T*)GetUObject();
    }

    FORCEINLINE bool operator==(const TObjectIterator& Rhs) const { return Index == Rhs.Index; }
    FORCEINLINE bool operator!=(const TObjectIterator& Rhs) const { return Index != Rhs.Index; }

protected:
    UObject* GetUObject() const
    {
        return ObjectArray[Index];
    }

    bool Advance()
    {
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
    /** Results from the GetObjectsOfClass query */
    TArray<UObject*> ObjectArray;
    int32 Index;
};


/**
 * 특정 타입의 UObject 인스턴스들을 순회할 수 있는 범위를 정의하는 구조체입니다.
 * 이 구조체는 range-based for 루프에서 사용될 수 있습니다.
 *
 * @tparam T 순회할 UObject 타입 또는 그 파생 클래스
 */
template <typename T>
struct TObjectRange
{
    TObjectRange(bool bIncludeDerivedClasses = true)
        : Begin(bIncludeDerivedClasses)
    {
    }

    friend TObjectIterator<T> begin(const TObjectRange& Range) { return Range.Begin; }
    friend TObjectIterator<T> end(const TObjectRange& Range) { return TObjectIterator<T>(TObjectIterator<T>::EndTag, Range.Begin); }

    TObjectIterator<T> Begin;
};
