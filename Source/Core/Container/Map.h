#pragma once
#include <unordered_map>
#include <utility>
#include <algorithm>
#include "ContainerAllocator.h"
#include "Pair.h"
#include "Serialization/Archive.h"


template <typename KeyType, typename ValueType, typename Allocator = FDefaultAllocator<std::pair<const KeyType, ValueType>>>
class TMap
{
public:
    using PairType = TPair<const KeyType, ValueType>;
    using MapType = std::unordered_map<KeyType, ValueType, std::hash<KeyType>, std::equal_to<KeyType>, Allocator>;
    using SizeType = typename MapType::size_type;
    using Iterator = typename MapType::iterator;
    using ConstIterator = typename MapType::const_iterator;

private:
    MapType PrivateMap;
public:
    // TPair를 반환하는 커스텀 반복자
    Iterator begin() noexcept { return Iterator(PrivateMap.begin()); }
    Iterator end() noexcept { return Iterator(PrivateMap.end()); }
    ConstIterator begin() const noexcept { return ConstIterator(PrivateMap.begin()); }
    ConstIterator end() const noexcept { return ConstIterator(PrivateMap.end()); }

    // 생성자 및 소멸자
    TMap() = default;
    ~TMap() = default;

    // 복사 생성자
    TMap(const TMap& Other) : PrivateMap(Other.PrivateMap) {}

    // 이동 생성자
    TMap(TMap&& Other) noexcept : PrivateMap(std::move(Other.PrivateMap)) {}

    // 복사 할당 연산자
    TMap& operator=(const TMap& Other)
    {
        if (this != &Other)
        {
            PrivateMap = Other.PrivateMap;
        }
        return *this;
    }

    // 이동 할당 연산자
    TMap& operator=(TMap&& Other) noexcept
    {
        if (this != &Other)
        {
            PrivateMap = std::move(Other.PrivateMap);
        }
        return *this;
    }

    // 요소 접근 및 수정
    ValueType& operator[](const KeyType& Key)
    {
        return PrivateMap[Key];
    }

    const ValueType& operator[](const KeyType& Key) const
    {
        return PrivateMap.at(Key);
    }

    void Add(const KeyType& Key, const ValueType& Value)
    {
        PrivateMap.insert_or_assign(Key, Value);
    }

    void Emplace(KeyType&& Key, ValueType&& Value)
    {
        PrivateMap.emplace(std::move(Key), std::move(Value));
    }

    void Remove(const KeyType& Key)
    {
        PrivateMap.erase(Key);
    }

    void Empty()
    {
        PrivateMap.clear();
    }

    // 검색 및 조회
    bool Contains(const KeyType& Key) const
    {
        return PrivateMap.find(Key) != PrivateMap.end();
    }

    const ValueType* Find(const KeyType& Key) const
    {
        auto it = PrivateMap.find(Key);
        return it != PrivateMap.end() ? &(it->second) : nullptr;
    }

    ValueType* Find(const KeyType& Key)
    {
        auto it = PrivateMap.find(Key);
        return it != PrivateMap.end() ? &(it->second) : nullptr;
    }

    // 크기 관련
    SizeType Num() const
    {
        return PrivateMap.size();
    }

    bool IsEmpty() const
    {
        return PrivateMap.empty();
    }

    // 용량 관련
    void Reserve(SizeType Number)
    {
        PrivateMap.reserve(Number);
    }

	void Serialize(FArchive& Ar) const
	{
		std::unordered_map<KeyType, ValueType> TempMap(PrivateMap.begin(), PrivateMap.end());
        Ar << TempMap;
	}
    void Deserialize(FArchive& Ar)
    {
		std::unordered_map<KeyType, ValueType> TempMap;
        Ar >> TempMap;
		PrivateMap = MapType(TempMap.begin(), TempMap.end());
    }
};
