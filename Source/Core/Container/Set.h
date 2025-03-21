#pragma once
#include <unordered_set>
#include "Array.h"
#include "ContainerAllocator.h"


template <typename T, typename Allocator = FDefaultAllocator<T>>
class TSet
{
private:
    using SetType = std::unordered_set<T, std::hash<T>, std::equal_to<T>, Allocator>;
    SetType PrivateSet;

public:
    using SizeType = typename Allocator::SizeType;
    using Iterator = typename SetType::iterator;
    using ConstIterator = typename SetType::const_iterator;

    // 기본 생성자
    TSet() = default;

    // Iterator 관련 메서드
    Iterator begin() noexcept { return PrivateSet.begin(); }
    Iterator end() noexcept { return PrivateSet.end(); }
    ConstIterator begin() const noexcept { return PrivateSet.begin(); }
    ConstIterator end() const noexcept { return PrivateSet.end(); }

    // CreateIterator 함수: 내부의 begin() iterator를 반환합니다.
    Iterator CreateIterator() noexcept { return PrivateSet.begin(); }
    ConstIterator CreateIterator() const noexcept { return PrivateSet.begin(); }

    
    // Add
    void Add(const T& Item) { PrivateSet.insert(Item); }

    // Emplace
    template<typename... Args>
    std::pair<Iterator, bool> Emplace(Args&&... args) 
    { 
        return PrivateSet.emplace(std::forward<Args>(args)...); 
    }

    // Num (개수)
    SizeType Num() const { return static_cast<SizeType>(PrivateSet.size()); }

    // Find
    Iterator Find(const T& Item) { return PrivateSet.find(Item); }
    ConstIterator Find(const T& Item) const { return PrivateSet.find(Item); }

    // Array (TArray로 반환)
    TArray<T, Allocator> Array() const
    {
        TArray<T, Allocator> Result;
        Result.Reserve(Num());
        for (const auto& Item : PrivateSet)
        {
            Result.Add(Item);
        }
        return Result;
    }

    // Remove
    SizeType Remove(const T& Item) { return static_cast<SizeType>(PrivateSet.erase(Item)); }

    // Empty
    void Empty() { PrivateSet.clear(); }

    // IsEmpty
    bool IsEmpty() const { return PrivateSet.empty(); }

    // GetAllocatedSize: 대략적인 할당된 메모리 크기를 반환합니다.
    size_t GetAllocatedSize() const
    {
        size_t Size = sizeof(*this);
        // 내부의 std::unordered_set이 사용하는 버킷들에 할당된 메모리 (대략적인 추정)
        Size += PrivateSet.bucket_count() * sizeof(void*);
        // 요소들이 차지하는 메모리 (실제 객체 저장 공간)
        Size += PrivateSet.size() * sizeof(T);
        return Size;
    }

    // Compact: 불필요한 버킷을 제거하여 메모리 사용을 최적화합니다.
    // 현재 요소 수에 맞춰 버킷을 재조정합니다.
    void Compact()
    {
        PrivateSet.rehash(PrivateSet.size());
    }


};