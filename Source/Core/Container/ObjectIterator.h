// TObjectIterator.h
#pragma once

#include "Core/Container/Map.h"      // TMap
#include <memory>

// 전방 선언: UObject와 원하는 타입(UStaticMesh 등)
class UObject;

// 템플릿 기반의 필터링 iterator
template <typename T>
class TObjectIterator
{
public:
    // UEngine::GObjects가 TMap<uint32, std::shared_ptr<UObject>> 타입이라고 가정
    using MapIterator = typename TMap<uint32, std::shared_ptr<UObject>>::Iterator;

    TObjectIterator(MapIterator InCurrent, MapIterator InEnd)
        : Current(InCurrent), End(InEnd)
    {
        SkipInvalid();
    }

    // 전위 증가
    TObjectIterator& operator++()
    {
        ++Current;
        SkipInvalid();
        return *this;
    }

    // 후위 증가
    TObjectIterator operator++(int)
    {
        TObjectIterator Temp(*this);
        ++(*this);
        return Temp;
    }

    // 비교 연산자
    bool operator==(const TObjectIterator& Other) const { return Current == Other.Current; }
    bool operator!=(const TObjectIterator& Other) const { return !(*this == Other); }

    // 역참조: 해당 요소가 원하는 타입의 객체라면 T*를 반환
    T* operator*() const
    {
        // dynamic_cast를 사용하여 T*로 캐스팅 (또는 내부 구현의 IsA() 활용)
        return dynamic_cast<T*>(Current->second.get());
    }

    // bool 변환: iterator가 유효하면 true (즉, Current != End)
    explicit operator bool() const { return Current != End; }

private:
    // 현재 iterator가 가리키는 요소가 원하는 타입인지 확인하고, 아니라면 건너뛰는 함수
    void SkipInvalid()
    {
        while (Current != End)
        {
            // 해당 객체가 존재하고, 원하는 타입(T)의 객체라면 중단
            if (Current->second && Current->second->IsA(T::StaticClass()))
            {
                break;
            }
            ++Current;
        }
    }

    MapIterator Current;
    MapIterator End;
};

// range-based for 문을 위한 헬퍼 클래스
template <typename T>
class TObjectRange
{
public:
    explicit TObjectRange(TMap<uint32, std::shared_ptr<UObject>>& InMap)
        : Map(InMap)
    {
    }

    auto begin() { return TObjectIterator<T>(Map.begin(), Map.end()); }
    auto end() { return TObjectIterator<T>(Map.end(), Map.end()); }

private:
    TMap<uint32, std::shared_ptr<UObject>>& Map;
};